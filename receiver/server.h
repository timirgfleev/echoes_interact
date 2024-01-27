#pragma once

#include "data.pb.h"
#include <iostream>
#include <boost/asio.hpp>

#include "device_parser_linux.h"

#include "permission_checker.h"
using boost::asio::ip::tcp;

class Server
{
public:
    Server(tcp::socket sk);

    ~Server();

    //
    void StartReceive();

protected:
    void send_callback(const boost::system::error_code &error, std::size_t bytes_transferred);
    void send_reply(coolProtocol::MessageWrapper reply_msg);

    void message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred);
    void handle_message(coolProtocol::MessageWrapper host_msg);
    void process_message(coolProtocol::MessageWrapper host_msg);

    void ping_pong();
    void get_device_info();

    void wait_for_reply(u_short deadline = 10);

    // these are error states
    void persimission_deny();
    void internal_error();
    void bad_data();
    void on_timeout();

    void close_connection();

private:
    Permission_Chercker permissions_;

    boost::asio::streambuf buffer_;
    tcp::socket sk_;
    boost::asio::deadline_timer timer_;
    bool is_deadline_set_;
};