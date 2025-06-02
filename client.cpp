#include "client.h"

SockerAutoStructClient::SockerAutoStructClient(const std::string& host, int port)
    : host(host), port(port), cli(host, port) {
    cli.set_connection_timeout(5);
}

template <typename TParam, typename TData>
bool SockerAutoStructClient::post(const std::string& route, const TParam& param, const TData& data) {
    std::string path = route;
    std::regex re("<\\w+>");
    path = std::regex_replace(path, re, std::to_string(param));

    try {
        nlohmann::json j = data;
        auto res = cli.Post(path.c_str(), j.dump(), "application/json");
        return res && res->status == 200;
    } catch (...) {
        return false;
    }
}

template bool SockerAutoStructClient::post<int, SimpleTest>(const std::string&, const int&, const SimpleTest&);
