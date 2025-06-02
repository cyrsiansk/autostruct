#include "server.h"

void SockerAutoStructServer::start(const std::string& host, int port) {
    registry.register_all(router);

    std::cout << "🚀 Server starting at " << host << ":" << port << std::endl;
    if (!svr.bind_to_port(host, port)) {
        throw std::runtime_error("Failed to bind server port");
    }
    svr.listen_after_bind();
}

void SockerAutoStructServer::stop() {
    svr.stop();
}
