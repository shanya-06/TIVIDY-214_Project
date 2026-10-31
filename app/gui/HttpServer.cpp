#include "HttpServer.h"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET SocketHandle;
typedef int SockLen;
#define CLOSE_SOCKET closesocket
#define SEND_FLAGS 0
#else
#include <arpa/inet.h>
#include <csignal>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
typedef int SocketHandle;
typedef socklen_t SockLen;
#define CLOSE_SOCKET close
#ifdef MSG_NOSIGNAL
#define SEND_FLAGS MSG_NOSIGNAL
#else
#define SEND_FLAGS 0
#endif
#endif

namespace {
const std::size_t MAX_REQUEST = 2 * 1024 * 1024;

std::string statusText(int status) {
    switch (status) {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        default: return "Internal Server Error";
    }
}

std::string lower(std::string s) {
    for (std::size_t i = 0; i < s.size(); ++i) s[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[i])));
    return s;
}

void sendAll(SocketHandle s, const std::string& data) {
    std::size_t sent = 0;
    while (sent < data.size()) {
        int n = send(s, data.c_str() + sent, static_cast<int>(data.size() - sent), SEND_FLAGS);
        if (n <= 0) return;
        sent += static_cast<std::size_t>(n);
    }
}
}  

HttpServer::HttpServer(int p) : port(p), listener(-1) {
#ifdef _WIN32
    WSADATA data;
    WSAStartup(MAKEWORD(2, 2), &data);
#else
    std::signal(SIGPIPE, SIG_IGN);  
#endif
}

HttpServer::~HttpServer() {
    if (listener != -1) CLOSE_SOCKET(static_cast<SocketHandle>(listener));
#ifdef _WIN32
    WSACleanup();
#endif
}

void HttpServer::open() {
    SocketHandle s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == static_cast<SocketHandle>(-1)) throw std::runtime_error("Could not create a socket");
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&yes), sizeof yes);

    sockaddr_in address;
    std::memset(&address, 0, sizeof address);
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<unsigned short>(port));
    address.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(s, reinterpret_cast<sockaddr*>(&address), sizeof address) != 0 || listen(s, 16) != 0) {
        CLOSE_SOCKET(s);
        throw std::runtime_error("Port " + std::to_string(port) + " is already in use. Close the other TIVIDY window or start with --port <number>.");
    }
    listener = static_cast<long long>(s);
}

void HttpServer::serve(const Handler& handler) {
    for (;;) {
        sockaddr_in peer;
        SockLen length = sizeof peer;
        SocketHandle client = accept(static_cast<SocketHandle>(listener), reinterpret_cast<sockaddr*>(&peer), &length);
        if (client == static_cast<SocketHandle>(-1)) continue;
        handleConnection(static_cast<long long>(client), handler);
        CLOSE_SOCKET(client);
    }
}

void HttpServer::handleConnection(long long handle, const Handler& handler) {
    SocketHandle client = static_cast<SocketHandle>(handle);
#ifdef _WIN32
    DWORD timeout = 5000;
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof timeout);
#else
    timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);
#endif

    // Read the head, then exactly Content-Length bytes of body.
    std::string data;
    char buffer[8192];
    std::size_t headEnd = std::string::npos;
    while (headEnd == std::string::npos) {
        int n = recv(client, buffer, sizeof buffer, 0);
        if (n <= 0) return;
        data.append(buffer, static_cast<std::size_t>(n));
        headEnd = data.find("\r\n\r\n");
        if (data.size() > MAX_REQUEST) return;
    }

    Request request;
    std::string head = data.substr(0, headEnd);
    std::size_t lineEnd = head.find("\r\n");
    std::string first = head.substr(0, lineEnd);
    std::size_t sp1 = first.find(' '), sp2 = first.find(' ', sp1 + 1);
    if (sp1 == std::string::npos || sp2 == std::string::npos) return;
    request.method = first.substr(0, sp1);
    std::string target = first.substr(sp1 + 1, sp2 - sp1 - 1);
    std::size_t q = target.find('?');
    request.path = target.substr(0, q);
    if (q != std::string::npos) request.query = target.substr(q + 1);

    std::size_t contentLength = 0;
    std::size_t pos = lineEnd;
    while (pos != std::string::npos && pos < head.size()) {
        std::size_t next = head.find("\r\n", pos + 2);
        std::string line = head.substr(pos + 2, next == std::string::npos ? std::string::npos : next - pos - 2);
        std::size_t colon = line.find(':');
        if (colon != std::string::npos && lower(line.substr(0, colon)) == "content-length")
            contentLength = static_cast<std::size_t>(std::strtoul(line.c_str() + colon + 1, nullptr, 10));
        pos = next;
    }

    Response response;
    if (contentLength > MAX_REQUEST) {
        response.status = 413;
        response.body = "{\"ok\":false,\"error\":\"Request too large\"}";
    } else {
        request.body = data.substr(headEnd + 4);
        while (request.body.size() < contentLength) {
            int n = recv(client, buffer, sizeof buffer, 0);
            if (n <= 0) return;
            request.body.append(buffer, static_cast<std::size_t>(n));
        }
        request.body.resize(contentLength);
        try {
            response = handler(request);
        } catch (const std::exception& e) {
            response = Response();
            response.status = 500;
            response.body = std::string("{\"ok\":false,\"error\":\"Server error\"}");
        }
    }

    std::string out = "HTTP/1.1 " + std::to_string(response.status) + " " + statusText(response.status) + "\r\n" +
                      "Content-Type: " + response.contentType + "\r\n" +
                      "Content-Length: " + std::to_string(response.body.size()) + "\r\n" +
                      "Cache-Control: no-store\r\n" +
                      "Connection: close\r\n\r\n" + response.body;
    sendAll(client, out);
}
