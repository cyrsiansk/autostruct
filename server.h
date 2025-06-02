#pragma once
#include "router.h"
#include "endpoint.h"
#include <vector>

class SockerAutoStructServer {
    httplib::Server svr;
    Router router;
    std::vector<std::shared_ptr<IEndpoint>> endpoints;

public:
    SockerAutoStructServer() : router(svr) {}

    template<typename TParam, typename TData>
    void bind(const std::string& route, std::function<void(TParam, TData)> cb) {
        auto ep = std::make_shared<Endpoint<TParam, TData>>(route, cb);
        add_endpoint(ep);
    }

    void add_endpoint(std::shared_ptr<IEndpoint> ep) {
        ep->bind_to(router);
        endpoints.push_back(std::move(ep));
    }

    void start(const std::string& host, int port);
    void stop();
};
