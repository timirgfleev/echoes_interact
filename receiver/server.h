#pragma once

#include "data.pb.h"
#include <iostream>
#include <boost/asio.hpp>

#include "device_parser_linux.h"

using boost::asio::ip::tcp;

class Server
{
public:
    Server(tcp::socket &sk);

    ~Server();

    //
    void StartReceive();

protected:
    void message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred);

    void send_callback(const boost::system::error_code &error, std::size_t bytes_transferred);

    void handle_message(coolProtocol::MessageWrapper host_msg);

    void send_reply(coolProtocol::MessageWrapper reply_msg);

    void ping_pong();

    void get_device_info();

    void wait_for_reply(u_short deadline = 10);

    void internal_error();
    void bad_data();

    void on_timeout();

    void close_connection();
    
    boost::asio::streambuf buffer_;
    coolProtocol::MessageWrapper expected_;
    tcp::socket &sk_;
};