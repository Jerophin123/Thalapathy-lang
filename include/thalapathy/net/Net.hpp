#pragma once
#include <string>
#include <map>
#include <functional>

namespace thalapathy {
namespace net {

#ifdef _WIN32
using socket_t = unsigned __int64;
#else
using socket_t = int;
#endif

// Parsed HTTP request handed to a THALAPATHY route handler.
struct HttpRequest {
    std::string method;   // GET, POST, ...
    std::string path;     // /users
    std::string query;    // raw query string (after '?')
    std::string body;     // request body
    std::map<std::string, std::string> headers;
    long long socketId = 0;
};

// Response a handler asks the server to send back.
struct HttpResponse {
    int status = 200;
    std::string contentType = "text/plain";
    std::string body;
    std::map<std::string, std::string> headers;
};

struct HttpResult {
    bool success = false;
    int status = 0;
    std::string body;
    std::string error;
};

// One-shot HTTP client. Supports http:// URLs (plaintext) on any port.
// verb is "GET" or "POST" etc. body is sent for non-GET verbs.
HttpResult httpRequest(const std::string& verb, const std::string& url,
                       const std::string& body = "",
                       const std::string& contentType = "application/json");

// Blocking HTTP/1.1 server. For each request, handler is invoked and its
// response written back. Returns false if the socket could not be bound.
// The loop runs until the process is terminated.
using RouteHandler = std::function<HttpResponse(const HttpRequest&)>;
bool httpServe(int port, const RouteHandler& handler);

extern std::function<bool()> g_reload_check;

bool wsUpgrade(socket_t client, const std::map<std::string, std::string>& headers);
bool wsSend(socket_t client, const std::string& message);
std::string wsRecv(socket_t client, bool& closed);

bool sseUpgrade(socket_t client);
bool sseSend(socket_t client, const std::string& data);

bool socketIsClosed(socket_t client);
void socketClose(socket_t client);

} // namespace net
} // namespace thalapathy
