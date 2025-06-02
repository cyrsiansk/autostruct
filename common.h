#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "serializable.h"

using json = nlohmann::json;

struct Proxy : Serializable<Proxy> {
    std::string protocol;
    std::string url;
    std::string login;
    std::string password;

    Proxy(const std::string& protocol_,
          const std::string& url_,
          const std::string& login_,
          const std::string& password_)
        : protocol(protocol_), url(url_), login(login_), password(password_) {}

    Proxy() = default;

    void to_json(json& j) const {
        j = json{
                {"protocol", protocol},
                {"url", url},
                {"login", login},
                {"password", password}
        };
    }

    void from_json(const json& j) {
        j.at("protocol").get_to(protocol);
        j.at("url").get_to(url);
        j.at("login").get_to(login);
        j.at("password").get_to(password);
    }
};

struct SimpleTest : Serializable<SimpleTest> {
    int retries;
    Proxy proxy;

    SimpleTest(int retries_, const Proxy& proxy_)
        : retries(retries_), proxy(proxy_) {}

    SimpleTest() = default;

    void to_json(json& j) const {
        j = json{
                {"retries", retries},
                {"proxy", proxy}
        };
    }

    void from_json(const json& j) {
        j.at("retries").get_to(retries);
        j.at("proxy").get_to(proxy);
    }
};
