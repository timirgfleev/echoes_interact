#include <iostream>
#include <boost/asio.hpp>

#include "server.h"

using boost::asio::ip::tcp;

// server class.
// it handles the socket connection when it is established

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
