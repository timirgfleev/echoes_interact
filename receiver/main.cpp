#include "data.pb.h"
#include <iostream>
#include <boost/asio.hpp>

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

        wait_for_reply(10);
    }

protected:
    void message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
    {
        if (!error)
        {
            //todo: maybe rewrite as one-liner
            std::istream is(&buffer_);
            coolProtocol::MessageWrapper msg;
            bool parsing_result = msg.ParseFromIstream(&is);
            if(parsing_result){
                handle_message(std::move(msg));
            } else {
                //todo: parse error
            }
        }
    }

    void handle_message(coolProtocol::MessageWrapper host_msg)
    {
        if (host_msg.has_request() && expected_.has_request())
        {
            switch (host_msg.request().command())
            {
            case coolProtocol::HostCommand::COMMAND_CONNECT:
                // todo:
                break;
            case coolProtocol::HostCommand::COMMAND_DISCONNECT:
                // todo:
                break;
            case coolProtocol::HostCommand::COMMAND_GET_DEVICE_INFO:
                // todo:
                break;
            default:
                break;
            }
        }
        else if (host_msg.has_pong() && expected_.has_pong())
        {
            // todo:
        }
        else
        {
            // host send weird stuff, close conn
        }
    }

    void send_reply(){
        
    }

    void ping_pong()
    {
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

    void on_timeout()
    {
        // close commection
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
