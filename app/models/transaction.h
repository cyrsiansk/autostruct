#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <serializable.h>

struct Transaction : Serializable<Transaction> {
    double amount;
    std::string currency;
    std::string status;

    Transaction() = default;
    Transaction(double a, std::string c, std::string s)
        : amount(a), currency(std::move(c)), status(std::move(s)) {}

    void to_json(nlohmann::json& j) const {
        j = {{"amount", amount}, {"currency", currency}, {"status", status}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("amount").get_to(amount);
        j.at("currency").get_to(currency);
        j.at("status").get_to(status);
    }
};
