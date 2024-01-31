#pragma once

#include "messages.pb.h"


namespace msgCreators
{
    typedef std::unique_ptr<coolProtocol::MessageWrapper> Message_UPTR;

    class Message
    {
    public:
        virtual ~Message() = default;
        virtual Message_UPTR create() = 0; // Pure virtual function
    };

    class Pong : public Message
    {
    public:
        Message_UPTR create() override
        {
            auto msg = std::make_unique<coolProtocol::MessageWrapper>();
            msg->set_allocated_pong(new coolProtocol::Pong());

            return msg;
        }
    };

    class Ping : public Message
    {
    public:
        Message_UPTR create() override
        {
            auto msg = std::make_unique<coolProtocol::MessageWrapper>();
            msg->set_allocated_ping(new coolProtocol::Ping());

            return msg;
        }
    };

    class CommandAbstract : public Message
    {

    public:
        virtual ~CommandAbstract() = default;
        Message_UPTR create() final
        {
            auto msg = std::make_unique<coolProtocol::MessageWrapper>();
            coolProtocol::HostCommand *host_command = new coolProtocol::HostCommand();
            host_command->set_command(command);
            msg->set_allocated_request(host_command);

            return msg;
        }

    protected:
        CommandAbstract(coolProtocol::HostCommand::Command command)
            : command(command)
        {
        }

        coolProtocol::HostCommand::Command command;
    };

    class CommandConnect : public CommandAbstract
    {
    public:
        CommandConnect() : CommandAbstract(coolProtocol::HostCommand::COMMAND_CONNECT)
        {
        }
    };

    class CommandDisconnect : public CommandAbstract
    {
    public:
        CommandDisconnect() : CommandAbstract(coolProtocol::HostCommand::COMMAND_DISCONNECT)
        {
        }
    };

    class CommandGetInfo : public CommandAbstract
    {
    public:
        CommandGetInfo() : CommandAbstract(coolProtocol::HostCommand::COMMAND_GET_DEVICE_INFO)
        {
        }
    };
}