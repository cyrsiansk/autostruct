#include "client.h"

SockerAutoStructClient::SockerAutoStructClient(const std::string& host, const int port)
    : host(host), port(port), cli(host, port) {
    cli.set_connection_timeout(5);
}

template <typename TParam, typename TData>
bool SockerAutoStructClient::post(const std::string& route, const TParam& param, const TData& data) {
    std::regex re("<\\w+>");
    std::string path = std::regex_replace(route, re, std::to_string(param));

    try {
        json j = data;
        auto res = cli.Post(path.c_str(), j.dump(), "application/json");
        return res && res->status == 200;
    } catch (...) {
        return false;
    }
}

template bool SockerAutoStructClient::post<int, SimpleTest>(const std::string&, const int&, const SimpleTest&);
