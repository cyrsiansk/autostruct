#pragma once
#include <string>
#include "router.h"

class IEndpoint {
public:
    virtual void bind_to(Router& router) = 0;
    virtual ~IEndpoint() = default;
};

template<typename TParam, typename TData>
class Endpoint : public IEndpoint {
    std::string route;
    std::function<void(TParam, TData)> handler;

public:
    Endpoint(std::string route, std::function<void(TParam, TData)> cb)
        : route(std::move(route)), handler(std::move(cb)) {}

    void bind_to(Router& router) override {
        router.bind<TParam, TData>(route, handler);
    }
};
