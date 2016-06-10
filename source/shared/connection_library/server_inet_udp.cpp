#include "server_inet_udp.hpp"

server_inet_udp::server_inet_udp(int portnr)
{
    this->port = portnr;
    this->continue_server = false;
}

server_inet_udp::~server_inet_udp()
{

}


void server_inet_udp::end()
{
    this->continue_server = false;
}


void server_inet_udp::run()
{

}
