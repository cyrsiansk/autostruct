#pragma once
#include "common.h"
#include <httplib.h>
#include <regex>
#include <string>

class SockerAutoStructClient {
    std::string host;
    int port;
    httplib::Client cli;

public:
    SockerAutoStructClient(const std::string& host, int port);

    template <typename TParam, typename TData>
    bool post(const std::string& route, TParam param, const TData& data) {
        std::string pattern = "<\\w+>";
        std::regex re(pattern);
        const std::string path = std::regex_replace(
            route,
            re,
            std::to_string(param)
        );

        try {
            json j = data;
            auto res = cli.Post(path.c_str(), j.dump(), "application/json");
            return res && res->status == 200;
        } catch (...) {
            return false;
        }
    }
};