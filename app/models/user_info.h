#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <serializable.h>

struct UserInfo : Serializable<UserInfo> {
    std::string name;
    std::string email;
    int age;

    UserInfo() = default;
    UserInfo(std::string n, std::string e, int a) : name(std::move(n)), email(std::move(e)), age(a) {}

    void to_json(nlohmann::json& j) const {
        j = {{"name", name}, {"email", email}, {"age", age}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("name").get_to(name);
        j.at("email").get_to(email);
        j.at("age").get_to(age);
    }
};