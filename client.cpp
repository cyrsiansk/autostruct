#include "client.h"

SockerAutoStructClient::SockerAutoStructClient(const std::string& host, const int port)
    : host(host), port(port), cli(host, port) 
{
    cli.set_connection_timeout(5);
}