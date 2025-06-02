#include "client.h"

SocketAutoStructClient::SocketAutoStructClient(const std::string& host, int port)
    : host(host), port(port), cli(host, port) {
    cli.set_connection_timeout(5);
}

template <typename TParam, typename TData>
bool SocketAutoStructClient::post(const std::string& route, const TParam& param, const TData& data) {
    std::string path = std::regex_replace(route, std::regex("<\\w+>"), std::to_string(param));
    try {
        nlohmann::json j = data;
        auto res = cli.Post(path.c_str(), j.dump(), "application/json");
        return res && res->status == 200;
    } catch (...) {
        return false;
    }
}


template bool SocketAutoStructClient::post<int, SimpleTest>(const std::string&, const int&, const SimpleTest&);
