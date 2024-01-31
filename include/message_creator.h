#pragma once

#include "messages.pb.h"
#include <memory>

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
        Message_UPTR create() override;
    };

    class Ping : public Message
    {
    public:
        Message_UPTR create() override;
    };

    class CommandAbstract : public Message
    {
    public:
        virtual ~CommandAbstract() = default;
        Message_UPTR create() final;

    protected:
        CommandAbstract(coolProtocol::HostCommand::Command command);

        coolProtocol::HostCommand::Command command;
    };

    class CommandConnect : public CommandAbstract
    {
    public:
        CommandConnect();
    };

    class CommandDisconnect : public CommandAbstract
    {
    public:
        CommandDisconnect();
    };

    class CommandGetInfo : public CommandAbstract
    {
    public:
        CommandGetInfo();
    };
}