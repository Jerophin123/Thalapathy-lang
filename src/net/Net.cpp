#include "thalapathy/net/Net.hpp"

#include <cstring>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <map>
#include <cstdint>

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <winsock2.h>
#  include <ws2tcpip.h>
   using socket_t = SOCKET;
#  define CLOSESOCK closesocket
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <unistd.h>
   using socket_t = int;
#  define INVALID_SOCKET (-1)
#  define CLOSESOCK ::close
#endif

namespace thalapathy {
namespace net {

std::function<bool()> g_reload_check = nullptr;

namespace {

struct SocketSystem {
    SocketSystem() {
#ifdef _WIN32
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
    }
    ~SocketSystem() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
};

// Ensure winsock is initialized exactly once for the process lifetime.
void ensureSockets() {
    static SocketSystem sys;
    (void)sys;
}

// Split a URL like http://host:port/path?query into pieces.
struct ParsedUrl {
    std::string host;
    int port = 80;
    std::string path = "/";
    bool ok = false;
};

ParsedUrl parseUrl(const std::string& url) {
    ParsedUrl p;
    std::string rest = url;
    const std::string httpPrefix = "http://";
    const std::string httpsPrefix = "https://";
    if (rest.rfind(httpPrefix, 0) == 0) {
        rest = rest.substr(httpPrefix.size());
    } else if (rest.rfind(httpsPrefix, 0) == 0) {
        // Plain-socket client cannot do TLS; refuse https clearly.
        return p;
    }

    size_t slash = rest.find('/');
    std::string hostPort = (slash == std::string::npos) ? rest : rest.substr(0, slash);
    p.path = (slash == std::string::npos) ? "/" : rest.substr(slash);

    size_t colon = hostPort.find(':');
    if (colon == std::string::npos) {
        p.host = hostPort;
        p.port = 80;
    } else {
        p.host = hostPort.substr(0, colon);
        try {
            p.port = std::stoi(hostPort.substr(colon + 1));
        } catch (...) {
            return p;
        }
    }
    p.ok = !p.host.empty();
    return p;
}

bool sendAll(socket_t sock, const std::string& data) {
    size_t sent = 0;
    while (sent < data.size()) {
        int n = ::send(sock, data.data() + sent, static_cast<int>(data.size() - sent), 0);
        if (n <= 0) return false;
        sent += static_cast<size_t>(n);
    }
    return true;
}

std::string recvUntilClose(socket_t sock) {
    std::string out;
    char buf[4096];
    for (;;) {
        int n = ::recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) break;
        out.append(buf, static_cast<size_t>(n));
    }
    return out;
}

// Read an HTTP request: request line + headers, then Content-Length bytes of body.
bool recvRequest(socket_t sock, std::string& raw) {
    char buf[4096];
    // Read until we have the header terminator.
    while (raw.find("\r\n\r\n") == std::string::npos) {
        int n = ::recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) return false;
        raw.append(buf, static_cast<size_t>(n));
        if (raw.size() > 8 * 1024 * 1024) break; // guard
    }
    // Determine content length.
    size_t headerEnd = raw.find("\r\n\r\n");
    if (headerEnd == std::string::npos) return true;
    std::string headerBlock = raw.substr(0, headerEnd);
    std::string lower = headerBlock;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    size_t clPos = lower.find("content-length:");
    size_t contentLength = 0;
    if (clPos != std::string::npos) {
        size_t valStart = clPos + std::string("content-length:").size();
        size_t lineEnd = lower.find("\r\n", valStart);
        std::string num = headerBlock.substr(valStart, lineEnd - valStart);
        try { contentLength = static_cast<size_t>(std::stoul(num)); } catch (...) { contentLength = 0; }
    }
    size_t haveBody = raw.size() - (headerEnd + 4);
    while (haveBody < contentLength) {
        int n = ::recv(sock, buf, sizeof(buf), 0);
        if (n <= 0) break;
        raw.append(buf, static_cast<size_t>(n));
        haveBody += static_cast<size_t>(n);
    }
    return true;
}

HttpRequest parseRequest(const std::string& raw) {
    HttpRequest req;
    size_t headerEnd = raw.find("\r\n\r\n");
    std::string head = (headerEnd == std::string::npos) ? raw : raw.substr(0, headerEnd);
    req.body = (headerEnd == std::string::npos) ? "" : raw.substr(headerEnd + 4);

    std::istringstream stream(head);
    std::string line;
    if (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        std::istringstream ls(line);
        std::string target;
        ls >> req.method >> target;
        size_t q = target.find('?');
        if (q == std::string::npos) {
            req.path = target;
        } else {
            req.path = target.substr(0, q);
            req.query = target.substr(q + 1);
        }
    }
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string val = line.substr(colon + 1);
            size_t s = val.find_first_not_of(" \t");
            if (s != std::string::npos) val = val.substr(s);
            req.headers[key] = val;
        }
    }
    return req;
}

const char* statusText(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 500: return "Internal Server Error";
        default:  return "OK";
    }
}

} // namespace

HttpResult httpRequest(const std::string& verb, const std::string& url,
                       const std::string& body, const std::string& contentType) {
    ensureSockets();
    HttpResult result;

    ParsedUrl u = parseUrl(url);
    if (!u.ok) {
        result.error = "invalid or unsupported URL (only http:// is supported)";
        return result;
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* res = nullptr;
    std::string portStr = std::to_string(u.port);
    if (getaddrinfo(u.host.c_str(), portStr.c_str(), &hints, &res) != 0 || !res) {
        result.error = "DNS resolution failed for host '" + u.host + "'";
        return result;
    }

    socket_t sock = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == INVALID_SOCKET) {
        freeaddrinfo(res);
        result.error = "socket creation failed";
        return result;
    }
    if (::connect(sock, res->ai_addr, static_cast<int>(res->ai_addrlen)) != 0) {
        freeaddrinfo(res);
        CLOSESOCK(sock);
        result.error = "connection to " + u.host + ":" + portStr + " failed";
        return result;
    }
    freeaddrinfo(res);

    std::ostringstream reqStream;
    reqStream << verb << " " << u.path << " HTTP/1.1\r\n";
    reqStream << "Host: " << u.host << "\r\n";
    reqStream << "User-Agent: THALAPATHY-TDK/1.0\r\n";
    reqStream << "Accept: */*\r\n";
    reqStream << "Connection: close\r\n";
    if (verb != "GET" && verb != "HEAD") {
        reqStream << "Content-Type: " << contentType << "\r\n";
        reqStream << "Content-Length: " << body.size() << "\r\n";
    }
    reqStream << "\r\n";
    if (verb != "GET" && verb != "HEAD") reqStream << body;

    if (!sendAll(sock, reqStream.str())) {
        CLOSESOCK(sock);
        result.error = "failed to send request";
        return result;
    }

    std::string response = recvUntilClose(sock);
    CLOSESOCK(sock);

    // Split status line + headers from body.
    size_t headerEnd = response.find("\r\n\r\n");
    std::string head = (headerEnd == std::string::npos) ? response : response.substr(0, headerEnd);
    result.body = (headerEnd == std::string::npos) ? "" : response.substr(headerEnd + 4);

    size_t sp = head.find(' ');
    if (sp != std::string::npos) {
        try { result.status = std::stoi(head.substr(sp + 1, 3)); } catch (...) { result.status = 0; }
    }
    result.success = result.status > 0;
    if (!result.success && result.error.empty()) {
        result.error = "malformed HTTP response";
    }
    return result;
}

bool httpServe(int port, const RouteHandler& handler) {
    ensureSockets();

    socket_t listener = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listener == INVALID_SOCKET) return false;

    int opt = 1;
    ::setsockopt(listener, SOL_SOCKET, SO_REUSEADDR,
                 reinterpret_cast<const char*>(&opt), sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<unsigned short>(port));

    if (::bind(listener, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) != 0) {
        CLOSESOCK(listener);
        return false;
    }
    if (::listen(listener, 16) != 0) {
        CLOSESOCK(listener);
        return false;
    }

    for (;;) {
        if (g_reload_check) {
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(listener, &fds);
            timeval tv{0, 500000}; // 500ms
            int sel = ::select(static_cast<int>(listener + 1), &fds, nullptr, nullptr, &tv);
            if (sel < 0) {
                CLOSESOCK(listener);
                return false;
            }
            if (g_reload_check()) {
                CLOSESOCK(listener);
                return true;
            }
            if (sel == 0) {
                continue;
            }
        }

        socket_t client = ::accept(listener, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;

        std::string raw;
        if (recvRequest(client, raw)) {
            HttpRequest req = parseRequest(raw);
            req.socketId = static_cast<long long>(client);
            HttpResponse resp;
            try {
                resp = handler(req);
            } catch (...) {
                resp.status = 500;
                resp.body = "Internal Server Error";
            }

            if (resp.status != -1) {
                std::ostringstream out;
                out << "HTTP/1.1 " << resp.status << " " << statusText(resp.status) << "\r\n";
                out << "Content-Type: " << resp.contentType << "\r\n";
                out << "Content-Length: " << resp.body.size() << "\r\n";
                for (const auto& [k, v] : resp.headers) {
                    out << k << ": " << v << "\r\n";
                }
                out << "Connection: close\r\n\r\n";
                out << resp.body;
                sendAll(client, out.str());
                CLOSESOCK(client);
            }
        } else {
            CLOSESOCK(client);
        }
    }
    // unreachable
}

bool recvExact(socket_t sock, char* buf, size_t len) {
    size_t received = 0;
    while (received < len) {
        int n = ::recv(sock, buf + received, static_cast<int>(len - received), 0);
        if (n <= 0) return false;
        received += static_cast<size_t>(n);
    }
    return true;
}

std::string sha1(const std::string& input) {
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476;
    uint32_t h4 = 0xC3D2E1F0;

    std::string message = input;
    uint64_t bitLength = message.size() * 8;
    message.push_back((char)0x80);
    while ((message.size() + 8) % 64 != 0) {
        message.push_back(0x00);
    }
    for (int i = 7; i >= 0; i--) {
        message.push_back((char)((bitLength >> (i * 8)) & 0xFF));
    }

    for (size_t offset = 0; offset < message.size(); offset += 64) {
        uint32_t w[80] = {0};
        for (int i = 0; i < 16; i++) {
            w[i] = ((uint32_t)(uint8_t)message[offset + i * 4] << 24)
                 | ((uint32_t)(uint8_t)message[offset + i * 4 + 1] << 16)
                 | ((uint32_t)(uint8_t)message[offset + i * 4 + 2] << 8)
                 | ((uint32_t)(uint8_t)message[offset + i * 4 + 3]);
        }
        for (int i = 16; i < 80; i++) {
            uint32_t val = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
            w[i] = (val << 1) | (val >> 31);
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;

        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
            e = d;
            d = c;
            c = (b << 30) | (b >> 2);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

    std::string digest;
    uint32_t hs[5] = {h0, h1, h2, h3, h4};
    for (int i = 0; i < 5; i++) {
        digest.push_back((char)((hs[i] >> 24) & 0xFF));
        digest.push_back((char)((hs[i] >> 16) & 0xFF));
        digest.push_back((char)((hs[i] >> 8) & 0xFF));
        digest.push_back((char)(hs[i] & 0xFF));
    }
    return digest;
}

std::string base64_encode(const std::string& in) {
    static const char lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    int val = 0, valb = -6;
    for (uint8_t c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(lookup[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(lookup[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4 != 0) out.push_back('=');
    return out;
}

bool wsUpgrade(socket_t client, const std::map<std::string, std::string>& headers) {
    std::string key = "";
    for (const auto& [k, v] : headers) {
        std::string lk = k;
        std::transform(lk.begin(), lk.end(), lk.begin(), ::tolower);
        if (lk == "sec-websocket-key") {
            key = v;
            break;
        }
    }
    if (key.empty()) return false;

    std::string acceptKey = base64_encode(sha1(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"));

    std::ostringstream ss;
    ss << "HTTP/1.1 101 Switching Protocols\r\n"
       << "Upgrade: websocket\r\n"
       << "Connection: Upgrade\r\n"
       << "Sec-WebSocket-Accept: " << acceptKey << "\r\n\r\n";

    return sendAll(client, ss.str());
}

bool wsSend(socket_t client, const std::string& message) {
    std::string header;
    header.push_back(static_cast<char>(0x81)); // FIN + Text

    size_t size = message.size();
    if (size <= 125) {
        header.push_back(static_cast<char>(size));
    } else if (size <= 65535) {
        header.push_back(static_cast<char>(126));
        header.push_back(static_cast<char>((size >> 8) & 0xFF));
        header.push_back(static_cast<char>(size & 0xFF));
    } else {
        header.push_back(static_cast<char>(127));
        for (int i = 7; i >= 0; i--) {
            header.push_back(static_cast<char>((size >> (i * 8)) & 0xFF));
        }
    }

    if (!sendAll(client, header)) return false;
    return sendAll(client, message);
}

std::string wsRecv(socket_t client, bool& closed) {
    closed = false;
    char header[2];
    if (!recvExact(client, header, 2)) {
        closed = true;
        return "";
    }

    uint8_t opcode = header[0] & 0x0F;
    bool masked = (header[1] & 0x80) != 0;
    uint64_t payloadLen = header[1] & 0x7F;

    if (opcode == 0x8) { // Close
        closed = true;
        return "";
    }

    if (payloadLen == 126) {
        char ext[2];
        if (!recvExact(client, ext, 2)) {
            closed = true;
            return "";
        }
        payloadLen = ((uint64_t)(uint8_t)ext[0] << 8) | (uint8_t)ext[1];
    } else if (payloadLen == 127) {
        char ext[8];
        if (!recvExact(client, ext, 8)) {
            closed = true;
            return "";
        }
        payloadLen = 0;
        for (int i = 0; i < 8; i++) {
            payloadLen = (payloadLen << 8) | (uint8_t)ext[i];
        }
    }

    char mask[4] = {0};
    if (masked) {
        if (!recvExact(client, mask, 4)) {
            closed = true;
            return "";
        }
    }

    std::vector<char> payload(static_cast<size_t>(payloadLen));
    if (payloadLen > 0) {
        if (!recvExact(client, payload.data(), static_cast<size_t>(payloadLen))) {
            closed = true;
            return "";
        }
    }

    if (masked) {
        for (size_t i = 0; i < payload.size(); i++) {
            payload[i] ^= mask[i % 4];
        }
    }

    return std::string(payload.begin(), payload.end());
}

bool sseUpgrade(socket_t client) {
    std::ostringstream ss;
    ss << "HTTP/1.1 200 OK\r\n"
       << "Content-Type: text/event-stream\r\n"
       << "Cache-Control: no-cache\r\n"
       << "Connection: keep-alive\r\n\r\n";
    return sendAll(client, ss.str());
}

bool sseSend(socket_t client, const std::string& data) {
    std::string message = "data: " + data + "\n\n";
    return sendAll(client, message);
}

bool socketIsClosed(socket_t client) {
    char buf;
#ifdef _WIN32
    int res = ::recv(client, &buf, 1, MSG_PEEK);
#else
    int res = ::recv(client, &buf, 1, MSG_PEEK | MSG_DONTWAIT);
#endif
    if (res == 0) return true;
    if (res < 0) {
#ifdef _WIN32
        int err = WSAGetLastError();
        if (err != WSAEWOULDBLOCK) return true;
#else
        if (errno != EAGAIN && errno != EWOULDBLOCK) return true;
#endif
    }
    return false;
}

void socketClose(socket_t client) {
    CLOSESOCK(client);
}

} // namespace net
} // namespace thalapathy
