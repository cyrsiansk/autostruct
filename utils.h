#pragma once
#include <string>
#include <sstream>
#include <stdexcept>

template<typename T>
T convert_param(const std::string& str) {
    std::istringstream iss(str);
    T value;
    if (!(iss >> value)) {
        throw std::invalid_argument("Failed to convert route parameter.");
    }
    return value;
}

template<>
inline std::string convert_param<std::string>(const std::string& str) {
    return str;
}
