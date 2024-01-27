#include "server.h"

Server::Server(tcp::socket sk)
    : sk_(std::move(sk)), expected_()
{
    
    expected_.set_allocated_request(new coolProtocol::HostCommand());
}

Server::~Server()
{
    std::cout << "Server destructor" << std::endl;
    expected_.Clear();
}

void Server::StartReceive()
{
    std::cout << "Server start" << std::endl;
    boost::asio::async_read_until(sk_, buffer_, '\0',
                                  std::bind(&Server::message_received_callback, this,
                                            std::placeholders::_1, std::placeholders::_2));
    std::cout << "Server start listen" << std::endl;
}

void Server::message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    std::cout << "Server received" << std::endl;
    if (!error)
    {
        std::istream is(&buffer_);
        std::string line(bytes_transferred, 0);
        is.read(&line[0], bytes_transferred); // sizeof(char) = 1
        coolProtocol::MessageWrapper msg;

        bool parsing_done = msg.ParseFromString(line);

        buffer_.consume(buffer_.size());

        //std::cout << "Server get{" << msg.DebugString() << '}' << std::endl;

        if (parsing_done)
        {
            std::cout << "Parsing done: " << parsing_done << std::endl;
            handle_message(std::move(msg));
        }
        else
        {
            bad_data();
        }
    }
    else
    {
        throw std::runtime_error("Server error");
    }
}

void Server::send_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    std::cout << "Server send callback" << std::endl;
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
        std::cout << "Server bad data in handle message" << std::endl;
        bad_data();
    }
}

void Server::send_reply(coolProtocol::MessageWrapper reply_msg)
{
    std::string serialized_msg;

    bool did_serialize = reply_msg.SerializeToString(&serialized_msg);

    std::cout << "Server sent:" << reply_msg.DebugString() << std::endl;

    if (did_serialize)
    {
        serialized_msg += '\n';
        boost::asio::async_write(sk_, boost::asio::buffer(serialized_msg), std::bind(&Server::send_callback, this, std::placeholders::_1, std::placeholders::_2));
    }
    else
    {
        internal_error();
    }
}

void Server::ping_pong()
{
    std::cout << "Server ping" << std::endl;
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
    std::cout << "Server timer" << std::endl;
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
    std::cout << "Server internal0 error" << std::endl;
    close_connection();
}

void Server::bad_data()
{
    std::cout << "Server bad data" << std::endl;
    close_connection();
}

void Server::on_timeout()
{
    close_connection();
}

void Server::close_connection()
{
    std::cout << "Server conn close" << std::endl;
    sk_.close();
}
