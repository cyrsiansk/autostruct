#pragma once
#include "common.h"
#include <httplib.h>
#include <regex>

class SockerAutoStructServer {
    httplib::Server svr;
    
public:
    template <typename TParam, typename TData, typename F>
    void bind(const std::string& route, F handler) {
        std::regex type_regex("<(\\w+)>");
        std::string pattern = std::regex_replace(route, type_regex, "([^/]+)");
        
        svr.Post(pattern, [=](const httplib::Request& req, httplib::Response& res) {
            std::smatch matches;
            if (!std::regex_search(req.path, matches, std::regex(pattern)) || matches.size() < 2) {
                res.status = 400;
                return;
            }
            
            try {
                TParam param = convert_param<TParam>(matches[1].str());
                TData data = json::parse(req.body).get<TData>();
                handler(param, data);
                res.status = 200;
            } catch (...) {
                res.status = 400;
            }
        });
    }
    
    void start(const std::string& host, int port);
    void stop();
};