#pragma once
#include "endpoint.h"
#include <vector>
#include <memory>

class EndpointRegistry {
    std::vector<std::shared_ptr<IEndpoint>> endpoints;

public:
    template<typename TParam, typename TData>
    void add(const std::string& route, std::function<void(TParam, TData)> cb) {
        auto ep = std::make_shared<Endpoint<TParam, TData>>(route, std::move(cb));
        endpoints.push_back(std::move(ep));
    }

    void register_all(Router& router) {
        for (const auto& ep : endpoints)
            ep->bind_to(router);
    }
};
