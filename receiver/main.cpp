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
        expected_.set_allocated_host_command(new coolProtocol::HostCommand());
    }

    //
    void StartReceive()
    {
        // listen for an inconimg messages
    }

protected:
    void handle_message(coolProtocol::HostCommand host_msg)
    {
    }

    void ping_pong()
    {
    }

    void wait_for_reply(u_short deadline = 10)
    {
        // boost::asio::deadline_timer timer(sk_.get_executor());

        // if not got message
        // on_timeout
        // else handle_message
    }

    void on_timeout()
    {
        // close commection
    }

    coolProtocol::MessageWrapper expected_;
    tcp::socket &sk_;
};

void handle_connection(tcp::socket socket)
{
    Server s(socket);
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
    coolProtocol::MessageWrapper messageWrapper;
    coolProtocol::HostCommand *hc = new coolProtocol::HostCommand();

    hc->set_command(coolProtocol::HostCommand::Command::HostCommand_Command_COMMAND_CONNECT);

    messageWrapper.set_allocated_host_command(hc);
    // tutorial::Person person;
    // person.set_name("zhangsan");
    // person.set_id(1);
    // person.set_email("123");

    std::cout << messageWrapper.IsInitialized();

    // delete hc;
}
