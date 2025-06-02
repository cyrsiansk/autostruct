#pragma once
#include "common.h"
#include <httplib.h>
#include <regex>
#include <functional>
#include <unordered_map>

class RouteHandlerBase {
public:
    virtual void handle(const httplib::Request& req, httplib::Response& res, const std::smatch& match) = 0;
    virtual ~RouteHandlerBase() = default;
};

template<typename TParam, typename TData>
class RouteHandler : public RouteHandlerBase {
    std::function<void(TParam, TData)> callback;

public:
    RouteHandler(std::function<void(TParam, TData)> cb) : callback(std::move(cb)) {}

    void handle(const httplib::Request& req, httplib::Response& res, const std::smatch& match) override {
        try {
            TParam param = convert_param<TParam>(match[1].str());
            TData data = json::parse(req.body).get<TData>();
            callback(param, data);
            res.status = 200;
        } catch (...) {
            res.status = 400;
        }
    }
};

class Router {
    httplib::Server& svr;
    std::vector<std::pair<std::regex, std::shared_ptr<RouteHandlerBase>>> routes;

public:
    Router(httplib::Server& s) : svr(s) {}

    template<typename TParam, typename TData>
    void bind(const std::string& route, std::function<void(TParam, TData)> cb) {
        std::regex param_re("<\\w+>");
        std::string pattern = std::regex_replace(route, param_re, "([^/]+)");
        auto compiled = std::regex(pattern);
        auto handler = std::make_shared<RouteHandler<TParam, TData>>(cb);

        svr.Post(pattern, [compiled, handler](const httplib::Request& req, httplib::Response& res) {
            std::smatch match;
            if (std::regex_search(req.path, match, compiled)) {
                handler->handle(req, res, match);
            } else {
                res.status = 400;
            }
        });

        routes.emplace_back(compiled, handler);
    }
};
