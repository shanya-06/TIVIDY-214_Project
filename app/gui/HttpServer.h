#ifndef HTTPSERVER_H
#define HTTPSERVER_H

/**
 * @file HttpServer.h
 * @brief A deliberately tiny HTTP/1.1 server (POSIX sockets / Winsock), so the
 * GUI needs no third-party library. It handles one request at a time, which
 * also means the engine is never called from two threads at once.
 * It only listens on 127.0.0.1: the GUI is for the local machine.
 */

#include <functional>
#include <map>
#include <string>

class HttpServer {
public:
    struct Request {
        std::string method, path, query, body;
    };
    struct Response {
        int status;
        std::string contentType, body;
        Response() : status(200), contentType("application/json; charset=utf-8") {}
    };
    typedef std::function<Response(const Request&)> Handler;

    explicit HttpServer(int port);
    ~HttpServer();

    /** @brief Binds and listens. @throws std::runtime_error if the port is in use. */
    void open();

    /** @brief Serves requests forever with handler. */
    void serve(const Handler& handler);

private:
    void handleConnection(long long client, const Handler& handler);

    int port;
    long long listener;
};

#endif
