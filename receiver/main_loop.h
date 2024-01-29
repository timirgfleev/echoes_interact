#pragma once
#include <boost/asio.hpp>
#include <../include/constants.h>
#include "server.h"

using boost::asio::ip::tcp;
typedef boost::asio::ip::port_type Port;

void listen_for(Port port, size_t exit_after);