#pragma once
#include <nlohmann/json.hpp>

template <typename Derived>
struct Serializable {
    friend void to_json(nlohmann::json& j, const Derived& d) {
        d.to_json(j);
    }

    friend void from_json(const nlohmann::json& j, Derived& d) {
        d.from_json(j);
    }
};
