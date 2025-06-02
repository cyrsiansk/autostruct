#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <serializable.h>

struct Product : Serializable<Product> {
    std::string name;
    double price;
    int stock;
    std::string category;

    Product() = default;
    Product(std::string n, double p, int s, std::string c)
        : name(std::move(n)), price(p), stock(s), category(std::move(c)) {}

    void to_json(nlohmann::json& j) const {
        j = {{"name", name}, {"price", price}, {"stock", stock}, {"category", category}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("name").get_to(name);
        j.at("price").get_to(price);
        j.at("stock").get_to(stock);
        j.at("category").get_to(category);
    }
};
