#include "data.pb.h"
#include <iostream>
#include <boost/asio.hpp>

#include "device_parser_linux.h"

using boost::asio::ip::tcp;

// server class.
// it handles the socket connection when it is established
class Server
{
public:
    Server(tcp::socket &sk)
        : sk_(sk), expected_()
    {
        expected_.set_allocated_request(new coolProtocol::HostCommand());
    }

    ~Server()
    {
        expected_.Clear();
    }

    //
    void StartReceive()
    {
        boost::asio::async_read(sk_, buffer_,
                                std::bind(&Server::message_received_callback, this,
                                          std::placeholders::_1, std::placeholders::_2));
    }

protected:
    void message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
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

    void send_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
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

    void handle_message(coolProtocol::MessageWrapper host_msg)
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

    void send_reply(coolProtocol::MessageWrapper reply_msg)
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

    void ping_pong()
    {
        coolProtocol::MessageWrapper ping;
        ping.set_allocated_ping(new coolProtocol::Ping);
        expected_.Clear();
        expected_.set_allocated_pong(new coolProtocol::Pong);
        send_reply(ping);
    }

    void get_device_info()
    {

        coolProtocol::DeviceInfo *d = new coolProtocol::DeviceInfo(DeviceParser::get_device_info());
        coolProtocol::MessageWrapper device_info;
        device_info.set_allocated_device_data(d);
        send_reply(device_info);
    }

    void wait_for_reply(u_short deadline = 10)
    {
        boost::asio::deadline_timer timer(sk_.get_executor(),
                                          boost::posix_time::seconds(deadline));

        timer.async_wait([this](const boost::system::error_code &error)
                         {
                            if (!error) {
                            this->on_timeout();
                            } });
    }

    void internal_error()
    {
        close_connection();
    }

    void bad_data()
    {
        close_connection();
    }

    void on_timeout()
    {
        close_connection();
    }

    void close_connection()
    {
        sk_.close();
    }
    boost::asio::streambuf buffer_;
    coolProtocol::MessageWrapper expected_;
    tcp::socket &sk_;
};

void handle_connection(tcp::socket socket)
{
    Server s(socket);
    s.StartReceive();
}

// main loop
void listen()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));

        for (;;)
        {
            tcp::socket socket(io_service);
            acceptor.accept(socket);
            auto p = std::thread(handle_connection, std::move(socket));
            p.detach();
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // listen();

    google::protobuf::ShutdownProtobufLibrary();
}
