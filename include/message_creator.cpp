#include "message_creator.h"

namespace msgCreators
{
    Message_UPTR Pong::create()
    {
        auto msg = std::make_unique<coolProtocol::MessageWrapper>();
        msg->set_allocated_pong(new coolProtocol::Pong());

        return msg;
    }

    Message_UPTR Ping::create()
    {
        auto msg = std::make_unique<coolProtocol::MessageWrapper>();
        msg->set_allocated_ping(new coolProtocol::Ping());

        return msg;
    }

    Message_UPTR CommandAbstract::create()
    {
        auto msg = std::make_unique<coolProtocol::MessageWrapper>();
        coolProtocol::HostCommand *host_command = new coolProtocol::HostCommand();
        host_command->set_command(command);
        msg->set_allocated_request(host_command);

        return msg;
    }

    CommandAbstract::CommandAbstract(coolProtocol::HostCommand::Command command)
        : command(command)
    {
    }

    CommandConnect::CommandConnect() : CommandAbstract(coolProtocol::HostCommand::COMMAND_CONNECT)
    {
    }

    CommandDisconnect::CommandDisconnect() : CommandAbstract(coolProtocol::HostCommand::COMMAND_DISCONNECT)
    {
    }

    CommandGetInfo::CommandGetInfo() : CommandAbstract(coolProtocol::HostCommand::COMMAND_GET_DEVICE_INFO)
    {
    }
}