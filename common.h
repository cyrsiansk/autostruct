#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <stdexcept>

using json = nlohmann::json;

struct Proxy {
    std::string protocol;
    std::string url;
    std::string login;
    std::string password;
};

struct SimpleTest {
    int retries;
    Proxy proxy;
};

template <typename T>
T convert_param(const std::string& str);

template <>
inline int convert_param<int>(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        throw std::runtime_error("Failed to convert param to int");
    }
}

template <>
inline float convert_param<float>(const std::string& str) {
    try {
        return std::stof(str);
    } catch (...) {
        throw std::runtime_error("Failed to convert param to float");
    }
}

template <>
inline std::string convert_param<std::string>(const std::string& str) {
    return str;
}

namespace nlohmann {
    inline void to_json(json& j, const Proxy& p) {
        j = json{
                {"protocol", p.protocol},
                {"url", p.url},
                {"login", p.login},
                {"password", p.password}
        };
    }

    inline void from_json(const json& j, Proxy& p) {
        j.at("protocol").get_to(p.protocol);
        j.at("url").get_to(p.url);
        j.at("login").get_to(p.login);
        j.at("password").get_to(p.password);
    }

    inline void to_json(json& j, const SimpleTest& s) {
        j = json{
                {"retries", s.retries},
                {"proxy", s.proxy}
        };
    }

    inline void from_json(const json& j, SimpleTest& s) {
        j.at("retries").get_to(s.retries);
        j.at("proxy").get_to(s.proxy);
    }
}