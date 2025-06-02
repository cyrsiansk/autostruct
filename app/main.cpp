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

#ifdef _WIN32
#include <windows.h>
#endif

// Генераторы случайных данных
std::string random_string(size_t length) {
    static const std::string chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<> dist(0, chars.size() - 1);

    std::string result;
    std::generate_n(std::back_inserter(result), length, []{ return chars[dist(gen)]; });
    return result;
}

double random_double(double min, double max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(min, max);
    return dist(gen);
}

int random_int(int min, int max) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(min, max);
    return dist(gen);
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::atomic server_running{true};
    const int MAX_CLIENTS = 50;
    const int REQUESTS_PER_CLIENT = 200;

    std::thread server_thread([&server_running] {
        try {
            SocketAutoStructServer server;

            // Оригинальные эндпоинты
            server.add_handler<int, SimpleTest>("/base/<int>/send", [](int type, const SimpleTest& data) {
                std::cout << "📧 [SimpleTest] type=" << type
                          << " retries=" << data.retries
                          << " proxy=" << data.proxy.url << std::endl;
            });

            server.add_handler<int, UserInfo>("/user/create", [](int useless_id, const UserInfo& user) {
                std::cout << "👤 [User] (useless_id=" << useless_id << ") name=" << user.name
                          << ", email=" << user.email
                          << ", age=" << user.age << std::endl;
            });

            server.add_handler<std::string, LogEntry>("/logs/<string>/add", [](const std::string& level, const LogEntry& log) {
                std::cout << "📘 [Log] [" << level << "] "
                          << log.timestamp << ": " << log.message << std::endl;
            });

            // Новые эндпоинты с бесполезными параметрами
            server.add_handler<int, Product>("/api/v1/products/<int>/add", [](int useless_flag, const Product& product) {
                std::cout << "🛒 [Product] (flag=" << useless_flag << ") Added: " << product.name
                          << " ($" << product.price << "), Stock: "
                          << product.stock << ", Category: "
                          << product.category << std::endl;
            });

            server.add_handler<std::string, Transaction>("/api/v1/transactions/<string>/process", [](const std::string& useless_token, const Transaction& tx) {
                std::cout << "💳 [Transaction] (token=" << useless_token << "), Amount: $" << tx.amount
                          << ", Currency: " << tx.currency
                          << ", Status: " << tx.status << std::endl;
            });

            server.add_handler<int, AnalyticsEvent>("/api/v1/analytics/<int>/collect", [](int useless_version, const AnalyticsEvent& event) {
                std::cout << "📊 [Analytics] (v=" << useless_version << ") " << event.event_name
                          << " from " << event.device_id
                          << ", payload: " << event.payload << std::endl;
            });

            server.add_handler<std::string, DeviceInfo>("/api/v1/devices/<string>/status", [](const std::string& device_id, const DeviceInfo& info) {
                std::cout << "📱 [Device] " << device_id << " (fw=" << info.firmware_version
                          << ", status=" << info.status
                          << ", battery=" << info.battery_level << "%)" << std::endl;
            });

            server.add_handler<int, WeatherData>("/api/v1/weather/<int>/update", [](int useless_region_code, const WeatherData& data) {
                std::cout << "☀️ [Weather] (region=" << useless_region_code << ") " << data.location
                          << ": " << data.temperature << "°C, "
                          << data.humidity << "% humidity, "
                          << data.pressure << " hPa" << std::endl;
            });

            // Убрали health check, так как он был GET

            server.start("0.0.0.0", 5000);
        } catch (const std::exception& e) {
            std::cerr << "⛔ Server error: " << e.what() << std::endl;
            server_running = false;
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (server_running) {
        std::vector<std::thread> clients;

        for (int i = 0; i < MAX_CLIENTS; ++i) {
            clients.emplace_back([i] {
                SocketAutoStructClient client("127.0.0.1", 5000);
                for (int j = 0; j < REQUESTS_PER_CLIENT; ++j) {
                    try {
                        int request_type = random_int(0, 7);
                        switch (request_type) {
                            case 0: { // Original SimpleTest
                                Proxy proxy{"socks5", "192.168.0." + std::to_string(random_int(1, 254)) + ":1080",
                                           "user" + random_string(4), "pass" + random_string(6)};
                                SimpleTest test{random_int(0, 5), proxy};
                                client.post("/base/<int>/send", random_int(1, 100), test);
                                break;
                            }
                            case 1: { // Original UserInfo
                                UserInfo user{
                                    "User-" + random_string(6),
                                    "user" + std::to_string(i*100 + j) + "@test.com",
                                    random_int(18, 65)
                                };
                                client.post("/user/create", random_int(0, 1), user);
                                break;
                            }
                            case 2: { // Original LogEntry
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
                            case 3: { // Product
                                Product p{
                                    "Product-" + random_string(5),
                                    random_double(10, 1000),
                                    random_int(1, 100),
                                    "Category-" + std::to_string(random_int(1, 5))
                                };
                                client.post("/api/v1/products/<int>/add", random_int(0, 3), p);
                                break;
                            }
                            case 4: { // Transaction
                                Transaction tx{
                                    random_double(1, 500),
                                    random_int(0, 1) ? "USD" : "EUR",
                                    random_int(0, 1) ? "completed" : "pending"
                                };
                                client.post("/api/v1/transactions/<string>/process", random_string(8), tx);
                                break;
                            }
                            case 5: { // Analytics
                                AnalyticsEvent event{
                                    "event-" + random_string(3),
                                    "device-" + random_string(8),
                                    R"({"param":")" + random_string(5) + R"("})"
                                };
                                client.post("/api/v1/analytics/<int>/collect", random_int(1, 3), event);
                                break;
                            }
                            case 6: { // Device info
                                std::string fw = std::to_string(random_int(1, 5)) + "." + std::to_string(random_int(0, 9));
                                DeviceInfo info{
                                    fw,
                                    random_int(0, 1) ? "online" : "offline",
                                    random_int(10, 100)
                                };
                                client.post("/api/v1/devices/<string>/status","dev-" + std::to_string(i), info);
                                break;
                            }
                            case 7: { // Weather data
                                WeatherData weather{
                                    "Location-" + random_string(4),
                                    random_double(-20, 40),
                                    random_double(30, 100),
                                    random_double(950, 1050)
                                };
                                client.post("/api/v1/weather/<int>/update", random_int(1, 10), weather);
                                break;
                            }
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(random_int(50, 300)));
                    } catch (const std::exception& e) {
                        std::cerr << "Client " << i << " error: " << e.what() << std::endl;
                    }
                }
            });
        }

        // Убрали health check thread, так как он использовал GET

        for (auto& t : clients) {
            if (t.joinable()) t.join();
        }

        server_running = false;
    }

    server_thread.join();
    return 0;
}