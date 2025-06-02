#pragma once
#include "router.h"

class SockerAutoStructServer {
    httplib::Server svr;
    Router router;

public:
    SockerAutoStructServer() : router(svr) {}

    template<typename TParam, typename TData>
    void bind(const std::string& route, std::function<void(TParam, TData)> cb) {
        router.bind(route, cb);
    }

    void start(const std::string& host, int port);
    void stop();
};
