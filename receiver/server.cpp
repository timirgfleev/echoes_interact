#include "server.h"

Server::Server(tcp::socket &sk)
    : sk_(sk), expected_()
{
    expected_.set_allocated_request(new coolProtocol::HostCommand());
}

Server::~Server()
{
    expected_.Clear();
}


void Server::StartReceive()
{
    boost::asio::async_read(sk_, buffer_,
                            std::bind(&Server::message_received_callback, this,
                                      std::placeholders::_1, std::placeholders::_2));
}

void Server::message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error)
    {
        // todo: maybe rewrite as one-liner
        std::istream is(&buffer_);
        coolProtocol::MessageWrapper msg;
        bool parsing_done = msg.ParseFromIstream(&is);

        buffer_.consume(buffer_.size());

        if (parsing_done)
        {
            handle_message(std::move(msg));
        }
        else
        {
            bad_data();
        }
    }
}

void Server::send_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error)
    {
        StartReceive();
    }
    else
    {
        internal_error();
    }
}

void Server::handle_message(coolProtocol::MessageWrapper host_msg)
{
    if (host_msg.has_request() && expected_.has_request())
    {
        switch (host_msg.request().command())
        {
        case coolProtocol::HostCommand::COMMAND_CONNECT:
            ping_pong();
            break;
        case coolProtocol::HostCommand::COMMAND_DISCONNECT:
            close_connection();
            break;
        case coolProtocol::HostCommand::COMMAND_GET_DEVICE_INFO:
            get_device_info();
            break;
        default:
            bad_data();
            break;
        }
    }
    else if (host_msg.has_pong() && expected_.has_pong())
    {
        expected_.clear_message();
        expected_.set_allocated_request(new coolProtocol::HostCommand());
    }
    else
    {
        bad_data();
    }
}

void Server::send_reply(coolProtocol::MessageWrapper reply_msg)
{
    std::string serialized_msg;

    std::ostream ost(&buffer_);

    bool did_serialize = reply_msg.SerializePartialToOstream(&ost);

    if (did_serialize)
    {

        boost::asio::async_write(sk_, buffer_, std::bind(&Server::send_callback, this, std::placeholders::_1, std::placeholders::_2));
    }
    else
    {
        internal_error();
    }
}

void Server::ping_pong()
{
    coolProtocol::MessageWrapper ping;
    ping.set_allocated_ping(new coolProtocol::Ping);
    expected_.Clear();
    expected_.set_allocated_pong(new coolProtocol::Pong);
    send_reply(ping);
    wait_for_reply();
}

void Server::get_device_info()
{

    coolProtocol::DeviceInfo *d = new coolProtocol::DeviceInfo(DeviceParser::get_device_info());
    coolProtocol::MessageWrapper device_info;
    device_info.set_allocated_device_data(d);
    send_reply(device_info);
}

void Server::wait_for_reply(u_short deadline)
{
    boost::asio::deadline_timer timer(sk_.get_executor(),
                                      boost::posix_time::seconds(deadline));

    timer.async_wait([this](const boost::system::error_code &error)
                     {
                            if (!error) {
                            this->on_timeout();
                            } });
}

void Server::internal_error()
{
    close_connection();
}

void Server::bad_data()
{
    close_connection();
}

void Server::on_timeout()
{
    close_connection();
}

void Server::close_connection()
{
    sk_.close();
}
