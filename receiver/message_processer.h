#pragma once

#include <memory>

#include "../include/message_creator.h"

#include "device_parser_linux.h"
#include "permission_checker.h"
#include "../include/constants.h"

/*
 * MessageProcesser is responsible for handling message
 * It does check permissions and call corresponding functions
 * That way reply is generated (or none via nullptr)
 * It requests deadline for reply, if processed message implies it
 * It also gets into error state, if something goes wrong
 */
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

    msgCreators::Message_UPTR handle_message(coolProtocol::MessageWrapper host_msg);

    ProcessingState get_state() const;

    bool is_deadline_set() const;

protected:
    msgCreators::Message_UPTR process_message(coolProtocol::MessageWrapper host_msg);

    msgCreators::Message_UPTR ping_pong();
    msgCreators::Message_UPTR get_device_info() const;
    void disconnect();

private:
    ProcessingState state_;
    bool deadline_set_;
    Permission_Chercker permissions_;
};