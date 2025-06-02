#pragma once
#include "router.h"
#include "endpoint_registry.h"

class SocketAutoStructServer {
    httplib::Server svr;
    Router router;
    EndpointRegistry registry;

public:
    SocketAutoStructServer() : router(svr) {}

    template<typename TParam, typename TData>
    void add_handler(const std::string& route, std::function<void(TParam, TData)> cb) {
        registry.add(route, std::move(cb));
    }

    void start(const std::string& host, int port);
    void stop();
};
