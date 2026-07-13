#include "thalapathy/net/Net.hpp"

#include <cstring>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <algorithm>

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
        socket_t client = ::accept(listener, nullptr, nullptr);
        if (client == INVALID_SOCKET) continue;

        std::string raw;
        if (recvRequest(client, raw)) {
            HttpRequest req = parseRequest(raw);
            HttpResponse resp;
            try {
                resp = handler(req);
            } catch (...) {
                resp.status = 500;
                resp.body = "Internal Server Error";
            }

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
        }
        CLOSESOCK(client);
    }
    // unreachable
}

} // namespace thalapathy
} // namespace thalapathy
