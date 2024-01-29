#include "main_loop.h"

std::unique_ptr<Server> handle_connection(tcp::socket socket)
{
    std::unique_ptr<Server> s = std::make_unique<Server>(std::move(socket));
    return s;
}

void listen_for(Port port, size_t exit_after)
{
    try
    {
        boost::asio::io_service io_service;
        // boost::asio::io_service::work work(io_service)
        // std::thread io_thread([&io_service]() { io_service.run(); });

        tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port));

        std::vector<std::unique_ptr<Server>> v;
        for (;;)
        {

            tcp::socket socket(io_service);
            std::cout << "Ready for connections" << std::endl;
            acceptor.accept(socket);
            std::cout << "Connection received" << std::endl;

            std::unique_ptr<Server> srv = handle_connection(std::move(socket));
            srv->StartReceive();

            io_service.run();
            io_service.reset();

            std::cout << "Server ends with code: " << static_cast<int>(srv->get_state()) << std::endl;

            static size_t i = 0;
            std::cout << i++ << " io service dead\n\n"
                      << std::endl;
            if (i == exit_after)
            {
                break;
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}