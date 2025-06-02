#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <serializable.h>

struct LogEntry : Serializable<LogEntry> {
    std::string timestamp;
    std::string message;

    LogEntry() = default;
    LogEntry(std::string t, std::string m) : timestamp(std::move(t)), message(std::move(m)) {}

    void to_json(nlohmann::json& j) const {
        j = {{"timestamp", timestamp}, {"message", message}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("timestamp").get_to(timestamp);
        j.at("message").get_to(message);
    }
};