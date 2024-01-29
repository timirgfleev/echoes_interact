#pragma once

#include <boost/asio.hpp>
#include <memory>

#include "data.pb.h"

#include "device_parser_linux.h"
#include "permission_checker.h"
#include "../include/constants.h"

class MessageProcesser
{
public:
    // these are error states
    enum class ProcessingState
    {
        SUCCESS = 0,
        PERMISSION_DENIED = 1,
        UNKNOWN_MESSAGE = 2,
        INTERNAL_ERROR = 3,
        CONNECTION_CLOSE = 4,
    };

    MessageProcesser();

    std::unique_ptr<coolProtocol::MessageWrapper>
    handle_message(coolProtocol::MessageWrapper host_msg);

    ProcessingState get_state();

    bool is_deadline_set();

protected:
    std::unique_ptr<coolProtocol::MessageWrapper> process_message(coolProtocol::MessageWrapper host_msg);

    std::unique_ptr<coolProtocol::MessageWrapper> ping_pong();

    std::unique_ptr<coolProtocol::MessageWrapper> get_device_info();

private:
    ProcessingState state_;
    bool deadline_set_;
    Permission_Chercker permissions_;
};