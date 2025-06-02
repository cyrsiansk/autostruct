#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <serializable.h>

struct DeviceInfo : Serializable<DeviceInfo> {
    std::string firmware_version;
    std::string status;
    int battery_level;

    DeviceInfo() = default;
    DeviceInfo(std::string fw, std::string st, int bl)
        : firmware_version(std::move(fw)), status(std::move(st)), battery_level(bl) {}

    void to_json(nlohmann::json& j) const {
        j = {{"firmware_version", firmware_version}, {"status", status}, {"battery_level", battery_level}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("firmware_version").get_to(firmware_version);
        j.at("status").get_to(status);
        j.at("battery_level").get_to(battery_level);
    }
};
