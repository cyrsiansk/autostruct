#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <serializable.h>

struct Proxy : Serializable<Proxy> {
    std::string protocol;
    std::string url;
    std::string login;
    std::string password;

    Proxy() = default;
    Proxy(std::string p, std::string u, std::string l, std::string pw)
        : protocol(std::move(p)), url(std::move(u)), login(std::move(l)), password(std::move(pw)) {}

    void to_json(nlohmann::json& j) const {
        j = {{"protocol", protocol}, {"url", url}, {"login", login}, {"password", password}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("protocol").get_to(protocol);
        j.at("url").get_to(url);
        j.at("login").get_to(login);
        j.at("password").get_to(password);
    }
};
