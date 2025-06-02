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
    bool post(const std::string& route, const TParam& param, const TData& data);
};
