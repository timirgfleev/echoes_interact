#pragma once

#include "messages.pb.h"
#include <iostream>
#include <boost/asio.hpp>
#include <utility>

#include "../include/constants.h"

#include "message_processer.h"

using boost::asio::ip::tcp;

/*
 * Server class is responsible for communications with client.
 * It operates prefixed size messages (two step communicating process)
 * If received message is valid message, it is passed to MessageProcesser
 * If anythig goes wrong, server goes to corresponding error state
 */
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
    ServerError get_state() const;
    ~Server();

    // server loop below, listed in call order:
    void StartReceive();

protected:
    void size_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred);
    void message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred);

    // this one is called to look for need of going to exit point
    bool process_erors_is_continue(MessageProcesser::ProcessingState error);
    // setup auto exit point
    void wait_for_reply(u_short deadline = config::TIMEOT_LEN);

    void send_callback(const boost::system::error_code &error, std::size_t bytes_transferred);
    void send_reply(coolProtocol::MessageWrapper reply_msg);
    // server loop above

    // these two are exit points from server loop
    void on_timeout();
    void close_connection();

private:
    ServerError state_;

    // sk_ has to be initialized before or timer_
    tcp::socket sk_;

    config::MESSAGE_SIZE next_message_size_;
    boost::asio::streambuf buffer_;

    MessageProcesser msg_processer_;

    boost::asio::deadline_timer timer_;
    bool is_deadline_set_;
};
