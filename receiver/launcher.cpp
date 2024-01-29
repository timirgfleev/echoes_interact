#include "launcher.h"
#include "../include/constants.h"
std::unique_ptr<Server> handle_connection(tcp::socket socket)
{
    std::unique_ptr<Server> s = std::make_unique<Server>(std::move(socket));
    return s;
}

// main loop
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
            std::cout << "conn received" << std::endl;
            std::unique_ptr<Server> srv =
                handle_connection(std::move(socket));
            srv->StartReceive();
            // v.push_back(std::move(srv));
            // v[v.size() - 1]->StartReceive();

            io_service.run();

            static size_t i = 0;
            std::cout << i++ << "io service dead\n\n"
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

void launcher(int argc, char *argv[])
{
    try
    {
        switch (argc)
        {
        case 2:
            if (std::string(argv[1]) == "-r")
            {
                listen_for(user_constant::DEF_PORT, 3);
            }
            break;
        case 3:
            if (std::string(argv[1]) == "-p")
            {
                listen_for(std::stoi(argv[2]), 3);
            }
            else if (std::string(argv[1]) == "-e")
            {
                listen_for(user_constant::DEF_PORT, std::stoi(argv[2]));
            }
            else
            {
                throw std::exception();
            }
            break;
        case 5:
            if (std::string(argv[1]) == "-p" && std::string(argv[3]) == "-e")
            {
                listen_for(std::stoi(argv[2]), std::stoi(argv[4]));
            }
            else if (std::string(argv[1]) == "-e" && std::string(argv[3]) == "-p")
            {
                listen_for(std::stoi(argv[4]), std::stoi(argv[2]));
            }
            else
            {
                throw std::exception();
            }
            break;
        default:
            throw std::exception();
            break;
        }
    }
    catch (std::exception &e)
    {
        std::cout << "USAGE:           [optional] -p <port>\n"
                  << "                 [optional] -e <number> of connections to accept before exiting\n"
                  << "[if none optional selected] -r run for 3 connections and on port 1234\n"
                  << std::endl;
    }
}