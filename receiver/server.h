#pragma once

#include "data.pb.h"
#include <iostream>
#include <boost/asio.hpp>
#include <utility>

#include "../include/constants.h"

#include "message_processor.h"

using boost::asio::ip::tcp;

class Server
{
public:
    enum class ServerError
    {
        OK = 0,
        INTERNAL_ERROR = 1,
        INTERNAL_PROCESS_ERROR = 2,
        BAD_DATA = 3,
        BAD_BEHAVIOR = 4,
        TIMEOUT = 5,
        CONNECTION_CLOSE = 6,
        UNKNOWN_ERROR = 7,
    };
    Server(tcp::socket sk);

    ~Server();

    void StartReceive();

protected:
    void send_callback(const boost::system::error_code &error, std::size_t bytes_transferred);
    void send_reply(coolProtocol::MessageWrapper reply_msg);

    void size_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred);
    void message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred);

    void wait_for_reply(u_short deadline = user_constant::TIMEOT_LEN);
    void on_timeout();

    // std::unique_ptr<coolProtocol::MessageWrapper>
    // get_device_info();

    void close_connection();
    bool is_continue_state(MessageProcesser::ProcessingState error);

private:
    ServerError state_;

    MessageProcesser msg_processer_;
    user_constant::MESSAGE_SIZE next_message_size_;
    boost::asio::streambuf buffer_;
    tcp::socket sk_;

    boost::asio::deadline_timer timer_;
    bool is_deadline_set_;
};