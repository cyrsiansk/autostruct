#pragma once
#include "iroute_handler.h"
#include <regex>
#include <vector>
#include <httplib.h>

class Router {
    httplib::Server& svr;

    struct Entry {
        std::regex regex;
        std::shared_ptr<IRouteHandler> handler;
    };

    std::vector<Entry> entries;

public:
    Router(httplib::Server& server) : svr(server) {}

    template<typename TParam, typename TData>
    void bind(const std::string& route, std::function<void(TParam, TData)> cb) {
        std::regex param_re("<\\w+>");
        std::string pattern = std::regex_replace(route, param_re, "([^/]+)");
        std::regex compiled(pattern);
        auto handler = std::make_shared<RouteHandlerImpl<TParam, TData>>(cb);

        svr.Post(pattern, [compiled, handler](const httplib::Request& req, httplib::Response& res) {
            std::smatch match;
            if (std::regex_search(req.path, match, compiled)) {
                bool ok = handler->handle(match[1].str(), req.body);
                res.status = ok ? 200 : 400;
            } else {
                res.status = 400;
            }
        });

        entries.push_back({compiled, handler});
    }
};
