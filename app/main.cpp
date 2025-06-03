#include <server.h>
#include <client.h>
#include "models/simple_test.h"
#include "models/user_info.h"
#include "models/log_entry.h"
#include "models/product.h"
#include "models/transaction.h"
#include "models/analytics_event.h"
#include "models/device_info.h"
#include "models/weather_data.h"
#include <thread>
#include <atomic>
#include <iostream>
#include <vector>
#include <random>
#include <mutex>

std::mutex cout_mutex;

#define COLOR_RESET   "\033[0m"
#define COLOR_HEADER  "\033[1;34m"
#define COLOR_FIELD   "\033[0;32m"
#define COLOR_VALUE   "\033[0;37m"
#define COLOR_ERROR   "\033[0;31m"
#define COLOR_WARNING "\033[0;33m"

#define LOG_BLOCK(tag) COLOR_HEADER "[" tag "]" COLOR_RESET
#define LOG_PAIR(field, value) COLOR_FIELD << field << "=" << COLOR_VALUE << value

#ifdef _WIN32
#include <windows.h>
#endif


void enable_virtual_terminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}


std::string random_string(const size_t length) {
    static const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string result;
    std::generate_n(std::back_inserter(result), length, []{ return chars[dist(gen)]; });
    return result;
}

double random_double(const double min, const double max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution dist(min, max);
    return dist(gen);
}

int random_int(const int min, const int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution dist(min, max);
    return dist(gen);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    enable_virtual_terminal();

    std::atomic server_running{true};
    constexpr int REQUESTS_PER_CLIENT = 1000;

    std::thread server_thread([&server_running] {
        try {
            SocketAutoStructServer server;

            server.add_handler<int, SimpleTest>("/base/<int>/send", [](const int type, const SimpleTest& data) {
            std::lock_guard lock(cout_mutex);
            std::cout << LOG_BLOCK("SimpleTest") << " "
                      << LOG_PAIR("type", type) << " | "
                      << LOG_PAIR("retries", data.retries) << " | "
                      << LOG_PAIR("proxy", data.proxy.url) << std::endl;
            });

            server.add_handler<int, UserInfo>("/user/create", [](const int id, const UserInfo& user) {
                std::lock_guard lock(cout_mutex);
                std::cout << LOG_BLOCK("UserCreate") << " "
                          << LOG_PAIR("id", id) << " | "
                          << LOG_PAIR("name", user.name) << " | "
                          << LOG_PAIR("email", user.email) << " | "
                          << LOG_PAIR("age", user.age) << std::endl;

                if (user.name.empty() || user.email.empty()) {
                    std::cout << COLOR_ERROR << "[Error] Missing required user fields" << COLOR_RESET << std::endl;
                }
            });

            server.add_handler<std::string, LogEntry>("/logs/<string>/add", [](const std::string& level, const LogEntry& log) {
                std::lock_guard lock(cout_mutex);
                std::cout << LOG_BLOCK("Log") << " "
                          << LOG_PAIR("level", level) << " | "
                          << LOG_PAIR("timestamp", log.timestamp) << " | "
                          << LOG_PAIR("message", log.message) << std::endl;

                if (log.message.find("deprecated") != std::string::npos) {
                    std::cout << COLOR_WARNING << "[Warning] Log message contains deprecated keyword" << COLOR_RESET << std::endl;
                }
            });

            server.add_handler<int, Product>("/api/v1/products/<int>/add", [](const int flag, const Product& product) {
                std::lock_guard lock(cout_mutex);
                std::cout << LOG_BLOCK("ProductAdd") << " "
                          << LOG_PAIR("flag", flag) << " | "
                          << LOG_PAIR("name", product.name) << " | "
                          << LOG_PAIR("price", "$" + std::to_string(product.price)) << " | "
                          << LOG_PAIR("stock", product.stock) << " | "
                          << LOG_PAIR("category", product.category) << std::endl;

                if (product.stock <= 0) {
                    std::cout << COLOR_WARNING << "[Warning] Product has no stock" << COLOR_RESET << std::endl;
                }
            });

            server.add_handler<std::string, Transaction>("/api/v1/transactions/<string>/process", [](const std::string& token, const Transaction& tx) {
                std::lock_guard lock(cout_mutex);
                std::cout << LOG_BLOCK("Transaction") << " "
                          << LOG_PAIR("token", token) << " | "
                          << LOG_PAIR("amount", "$" + std::to_string(tx.amount)) << " | "
                          << LOG_PAIR("currency", tx.currency) << " | "
                          << LOG_PAIR("status", tx.status) << std::endl;

                if (tx.amount <= 0) {
                    std::cout << COLOR_ERROR << "[Error] Invalid transaction amount" << COLOR_RESET << std::endl;
                }
            });

            server.add_handler<int, AnalyticsEvent>("/api/v1/analytics/<int>/collect", [](const int version, const AnalyticsEvent& event) {
                std::lock_guard lock(cout_mutex);
                std::cout << LOG_BLOCK("Analytics") << " "
                          << LOG_PAIR("version", version) << " | "
                          << LOG_PAIR("event", event.event_name) << " | "
                          << LOG_PAIR("device_id", event.device_id) << " | "
                          << LOG_PAIR("payload", event.payload) << std::endl;

                if (event.payload.empty()) {
                    std::cout << COLOR_WARNING << "[Warning] Analytics payload is empty" << COLOR_RESET << std::endl;
                }
            });

            server.add_handler<std::string, DeviceInfo>("/api/v1/devices/<string>/status", [](const std::string& device_id, const DeviceInfo& info) {
                std::lock_guard lock(cout_mutex);
                std::cout << LOG_BLOCK("DeviceStatus") << " "
                          << LOG_PAIR("id", device_id) << " | "
                          << LOG_PAIR("firmware", info.firmware_version) << " | "
                          << LOG_PAIR("status", info.status) << " | "
                          << LOG_PAIR("battery", info.battery_level) << "%" << std::endl;

                if (info.battery_level < 10) {
                    std::cout << COLOR_WARNING << "[Warning] Low battery level: " << info.battery_level << "%" << COLOR_RESET << std::endl;
                }
                if (info.battery_level < 5) {
                    std::cout << COLOR_ERROR << "[Error] Critical battery level: " << info.battery_level << "%" << COLOR_RESET << std::endl;
                }
            });

            server.add_handler<int, WeatherData>("/api/v1/weather/<int>/update", [](const int region_code, const WeatherData& data) {
                std::lock_guard lock(cout_mutex);
                std::cout << LOG_BLOCK("WeatherUpdate") << " "
                          << LOG_PAIR("region", region_code) << " | "
                          << LOG_PAIR("location", data.location) << " | "
                          << LOG_PAIR("temperature", data.temperature) << "°C | "
                          << LOG_PAIR("humidity", data.humidity) << "% | "
                          << LOG_PAIR("pressure", data.pressure) << " hPa" << std::endl;

                if (random_int(0, 50) == 42) {
                    std::cout << COLOR_ERROR << "[Error] Weather update failed" << COLOR_RESET << std::endl;
                    return;
                }

                if (data.temperature > 45) {
                    std::cout << COLOR_WARNING << "[Warning] Unusually high temperature: " << data.temperature << "°C" << COLOR_RESET << std::endl;
                }
                if (data.humidity > 80) {
                    std::cout << COLOR_WARNING << "[Warning] Unusually high humidity: " << data.humidity << "%" << COLOR_RESET << std::endl;
                }
                if (data.pressure > 1013) {
                    std::cout << COLOR_WARNING << "[Warning] Unusually high pressure: " << data.pressure << " hPa" << COLOR_RESET << std::endl;
                }
            });

            server.start("0.0.0.0", 5000);

        } catch (const std::exception& e) {
            std::cerr << "⛔ Server error: " << e.what() << std::endl;
            server_running = false;
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (server_running) {
        constexpr int MAX_CLIENTS = 100;
        std::vector<std::thread> clients;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            clients.emplace_back([i] {
                SocketAutoStructClient client("127.0.0.1", 5000);
                for (int j = 0; j < REQUESTS_PER_CLIENT; ++j) {
                    try {
                        switch (random_int(0, 7)) {
                            case 0: {
                                Proxy proxy{"socks5", "192.168.0." + std::to_string(random_int(1, 254)) + ":1080",
                                           "user" + random_string(4), "pass" + random_string(6)};
                                SimpleTest test{random_int(0, 5), proxy};
                                client.post("/base/<int>/send", random_int(1, 100), test);
                                break;
                            }
                            case 1: {
                                UserInfo user{
                                    "User-" + random_string(6),
                                    "user" + std::to_string(i*100 + j) + "@test.com",
                                    random_int(18, 65)
                                };
                                client.post("/user/create", random_int(0, 1), user);
                                break;
                            }
                            case 2: {
                                LogEntry log{
                                    "2025-06-0" + std::to_string(random_int(1, 9)) + "T" +
                                    std::to_string(random_int(10, 23)) + ":" +
                                    std::to_string(random_int(10, 59)) + ":" +
                                    std::to_string(random_int(10, 59)),
                                    "Log message #" + std::to_string(i*100 + j) + ": " + random_string(20)
                                };
                                client.post("/logs/<string>/add", random_int(0, 1) ? "info" : "error", log);
                                break;
                            }
                            case 3: {
                                Product p{
                                    "Product-" + random_string(5),
                                    random_double(10, 1000),
                                    random_int(1, 100),
                                    "Category-" + std::to_string(random_int(1, 5))
                                };
                                client.post("/api/v1/products/<int>/add", random_int(0, 3), p);
                                break;
                            }
                            case 4: {
                                Transaction tx{
                                    random_double(1, 500),
                                    random_int(0, 1) ? "USD" : "EUR",
                                    random_int(0, 1) ? "completed" : "pending"
                                };
                                client.post("/api/v1/transactions/<string>/process", random_string(8), tx);
                                break;
                            }
                            case 5: {
                                AnalyticsEvent event{
                                    "event-" + random_string(3),
                                    "device-" + random_string(8),
                                    R"({"param":")" + random_string(5) + R"("})"
                                };
                                client.post("/api/v1/analytics/<int>/collect", random_int(1, 3), event);
                                break;
                            }
                            case 6: {
                                std::string fw = std::to_string(random_int(1, 5)) + "." + std::to_string(random_int(0, 9));
                                DeviceInfo info{
                                    fw,
                                    random_int(0, 1) ? "online" : "offline",
                                    random_int(10, 100)
                                };
                                client.post("/api/v1/devices/<string>/status","dev-" + std::to_string(i), info);
                                break;
                            }
                            case 7: {
                                WeatherData weather{
                                    "Location-" + random_string(4),
                                    random_double(-20, 40),
                                    random_double(30, 100),
                                    random_double(950, 1050)
                                };
                                client.post("/api/v1/weather/<int>/update", random_int(1, 10), weather);
                                break;
                            }
                            default: ;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(random_int(10, 500)));
                    } catch (const std::exception& e) {
                        std::cerr << "Client " << i << " error: " << e.what() << std::endl;
                    }
                }
            });
        }

        for (auto& t : clients) {
            if (t.joinable()) t.join();
        }

        server_running = false;
    }

    server_thread.join();
    return 0;
}