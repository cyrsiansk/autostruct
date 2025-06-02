#include <server.h>
#include <client.h>
#include "models/simple_test.h"
#include <thread>
#include <chrono>
#include <atomic>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    std::atomic server_running{true};

    std::thread server_thread([&server_running] {
        try {
            SocketAutoStructServer server;
            server.add_handler<int, SimpleTest>("/base/<int>/send", [](int type, const SimpleTest& data) {
                std::cout << "📧 Received: type=" << type
                          << ", retries=" << data.retries
                          << ", proxy=" << data.proxy.url << std::endl;
            });
            server.start("0.0.0.0", 5000);
        } catch (const std::exception& e) {
            std::cerr << "⛔ Server error: " << e.what() << std::endl;
            server_running = false;
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (server_running) {
        SocketAutoStructClient client("localhost", 5000);
        int counter = 0;
        while (counter < 5) {
            Proxy proxy{"socks5", "127.0.0.1:1080", "user", "pass"};
            SimpleTest test{counter++, proxy};
            if (client.post("/base/<int>/send", 42, test)) {
                std::cout << "✅ Sent (retries=" << test.retries << ")" << std::endl;
            } else {
                std::cerr << "❌ Failed to send" << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        server_running = false;
    }

    server_thread.join();
    return 0;
}
