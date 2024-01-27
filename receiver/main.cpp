#include <iostream>
#include <boost/asio.hpp>
#include "server.h"

using boost::asio::ip::tcp;

// server class.
// it handles the socket connection when it is established

std::unique_ptr<Server> handle_connection(tcp::socket socket)
{
    std::unique_ptr<Server> s = std::make_unique<Server>(std::move(socket));
    return s;
}

// main loop
void listen()
{
    try
    {
        boost::asio::io_service io_service;
        boost::asio::io_service::work work(io_service);

        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234));

        //std::thread io_thread([&io_service]() { io_service.run(); });
        
        for (;;)
        {
            tcp::socket socket(io_service);
            std::cout << "Ready for connections" << std::endl;
            acceptor.accept(socket);
            std::cout << "conn received" << std::endl;
            std::unique_ptr<Server> srv = handle_connection(std::move(socket));
            srv->StartReceive();
            io_service.run();
            //todo: add thread pool
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

    listen();

    google::protobuf::ShutdownProtobufLibrary();
}
