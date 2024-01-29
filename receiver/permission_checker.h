#pragma once

#include "messages.pb.h"
#include <string>
#include <vector>

enum Permissions
{
    CONNECT,
    DISCONNECT,
    GET_DEVICE_INFO,
    PING,
    PONG
};

class Permission_Chercker
{
public:
    explicit Permission_Chercker(const std::vector<Permissions> &v)
        : expected_(std::move(v)){

          };

    ~Permission_Chercker() = default;

    void reset_permission()
    {
        expected_.clear();
    }

    void reset_permission(const std::vector<Permissions> &v)
    {
        expected_ = v;
    }

    void add_permission(const Permissions &p)
    {
        expected_.push_back(p);
    }

    bool check_permission(const Permissions &p) const
    {
        return std::count(expected_.begin(), expected_.end(), p);
    }

    bool check_permission(const coolProtocol::MessageWrapper &msg)
    {
        bool result = false;

        if (msg.has_request())
        {
            switch (msg.request().command())
            {
            case coolProtocol::HostCommand::COMMAND_CONNECT:
                result = check_permission(Permissions::CONNECT);
                break;
            case coolProtocol::HostCommand::COMMAND_DISCONNECT:
                result = check_permission(Permissions::DISCONNECT);
                break;
            case coolProtocol::HostCommand::COMMAND_GET_DEVICE_INFO:
                result = check_permission(Permissions::GET_DEVICE_INFO);
                break;
            default:
                //useless branch btw, but it is here for completeness
                result = false;
                break;
            }
        }
        else if (msg.has_pong())
        {
            result = check_permission(Permissions::PONG);
        }
        else if (msg.has_ping())
        {
            result = check_permission(Permissions::PING);
        }
        return result;
    }

private:
    // I use vector because vector(3) [for 3 elements] is faster than any (set, map, unordered_map)
    std::vector<Permissions> expected_;
};