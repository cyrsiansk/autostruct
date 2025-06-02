#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include <serializable.h>

struct WeatherData : Serializable<WeatherData> {
    std::string location;
    double temperature;
    double humidity;
    double pressure;

    WeatherData() = default;
    WeatherData(std::string loc, double t, double h, double p)
        : location(std::move(loc)), temperature(t), humidity(h), pressure(p) {}

    void to_json(nlohmann::json& j) const {
        j = {{"location", location}, {"temperature", temperature}, {"humidity", humidity}, {"pressure", pressure}};
    }

    void from_json(const nlohmann::json& j) {
        j.at("location").get_to(location);
        j.at("temperature").get_to(temperature);
        j.at("humidity").get_to(humidity);
        j.at("pressure").get_to(pressure);
    }
};
