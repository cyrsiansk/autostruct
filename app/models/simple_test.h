#pragma once
#include "proxy.h"
#include <serializable.h>

struct SimpleTest : Serializable<SimpleTest> {
    int retries;
    Proxy proxy;

    SimpleTest() = default;
    SimpleTest(int r, Proxy p) : retries(r), proxy(std::move(p)) {}

    void to_json(nlohmann::json& j) const {
        j = {{"retries", retries}, {"proxy", proxy}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("retries").get_to(retries);
        j.at("proxy").get_to(proxy);
    }
};
