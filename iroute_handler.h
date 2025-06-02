#pragma once
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include "utils.h"

class IRouteHandler {
public:
    virtual bool handle(const std::string& param_str, const std::string& body) = 0;
    virtual ~IRouteHandler() = default;
};

template<typename TParam, typename TData>
class RouteHandlerImpl : public IRouteHandler {
    std::function<void(TParam, TData)> cb;

public:
    RouteHandlerImpl(std::function<void(TParam, TData)> callback)
        : cb(std::move(callback)) {}

    bool handle(const std::string& param_str, const std::string& body) override {
        try {
            TParam param = convert_param<TParam>(param_str);
            TData data = nlohmann::json::parse(body);
            cb(param, data);
            return true;
        } catch (...) {
            return false;
        }
    }
};

