#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <serializable.h>

struct AnalyticsEvent : Serializable<AnalyticsEvent> {
    std::string event_name;
    std::string device_id;
    std::string payload;

    AnalyticsEvent() = default;
    AnalyticsEvent(std::string en, std::string did, std::string pl)
        : event_name(std::move(en)), device_id(std::move(did)), payload(std::move(pl)) {}

    void to_json(nlohmann::json& j) const {
        j = {{"event_name", event_name}, {"device_id", device_id}, {"payload", payload}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("event_name").get_to(event_name);
        j.at("device_id").get_to(device_id);
        j.at("payload").get_to(payload);
    }
};
