#include <iostream>
#include <boost/asio.hpp>
// #include "server.h"
#include "launcher.h"

using boost::asio::ip::tcp;

const Port DEF_PORT = Port(1234);

void launcher(int argc, char *argv[])
{
    try
    {
        switch (argc)
        {
        case 2:
            if (std::string(argv[1]) == "-r")
            {
                listen_for(DEF_PORT, 3);
            }
            break;
        case 3:
            if (std::string(argv[1]) == "-p")
            {
                listen_for(std::stoi(argv[2]), 3);
            }
            else if (std::string(argv[1]) == "-e")
            {
                listen_for(DEF_PORT, std::stoi(argv[2]));
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

int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    launcher(argc, argv);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}