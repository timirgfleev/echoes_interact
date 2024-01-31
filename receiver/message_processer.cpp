#include "message_processer.h"

MessageProcesser::MessageProcesser()
    : permissions_({Permissions::CONNECT, Permissions::DISCONNECT}),
      state_(ProcessingState::SUCCESS),
      deadline_set_(false)
{
}

std::unique_ptr<coolProtocol::MessageWrapper>
MessageProcesser::handle_message(coolProtocol::MessageWrapper host_msg)
{
    if (deadline_set_)
    {
        deadline_set_ = false;
    }

    std::unique_ptr<coolProtocol::MessageWrapper> response(nullptr);
    // PrintGreentext("Server received: " + host_msg.DebugString());
    bool has_permission = permissions_.check_permission(host_msg);

    if (has_permission)
    {
        response = process_message(std::move(host_msg));
    }
    else
    {
        state_ = ProcessingState::PERMISSION_DENIED;
    }
    return response;
}

std::unique_ptr<coolProtocol::MessageWrapper>
MessageProcesser::process_message(coolProtocol::MessageWrapper host_msg)
{
    std::unique_ptr<coolProtocol::MessageWrapper> response;

    if (host_msg.has_request())
    {

        switch (host_msg.request().command())
        {
        case coolProtocol::HostCommand::COMMAND_CONNECT:
            response = ping_pong();
            break;
        case coolProtocol::HostCommand::COMMAND_DISCONNECT:
            // no response, just close connection
            response = nullptr;
            disconnect();
            break;
        case coolProtocol::HostCommand::COMMAND_GET_DEVICE_INFO:
            response = get_device_info();
            break;
        default:
            state_ = ProcessingState::UNKNOWN_MESSAGE;
            break;
        }
    }
    else if (host_msg.has_pong())
    {
        // user is authorizedm let them do whatever they want
        // todo: maybe add a check for the state of already authorized
        std::cout << "Permission granted" << std::endl;
        permissions_.reset_permission({Permissions::CONNECT,
                                       Permissions::DISCONNECT,
                                       Permissions::GET_DEVICE_INFO});
        // no response to pong
    }
    else
    {
        state_ = ProcessingState::UNKNOWN_MESSAGE;
    }

    return response;
}

std::unique_ptr<coolProtocol::MessageWrapper>
MessageProcesser::ping_pong()
{
    std::cout << "Server ping" << std::endl;
    permissions_.reset_permission({Permissions::PONG});

    deadline_set_ = true;
    return msgCreators::Pong().create();
}

std::unique_ptr<coolProtocol::MessageWrapper>
MessageProcesser::get_device_info() const
{
    return msgCreators::DeviceInfo().create();
}

void MessageProcesser::disconnect()
{
    state_ = ProcessingState::CONNECTION_CLOSE;
}

bool MessageProcesser::is_deadline_set() const
{
    return deadline_set_;
}

MessageProcesser::ProcessingState
MessageProcesser::get_state() const
{
    return state_;
}