#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include "server.h"

typedef boost::asio::ip::port_type Port;

void listen_for(Port port, size_t exit_after);
void launcher(int argc, char *argv[]);
