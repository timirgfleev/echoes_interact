#include <iostream>
#include <boost/asio.hpp>
//#include "server.h"
#include "launcher.h"

using boost::asio::ip::tcp;


const Port DEF_PORT = Port(1234);


void launcher(int argc, char *argv[])
{
        try
    {

        switch (argc)
        {
        case 0:

            break;
        case 2:
            if (std::string(argv[0]) == "-r")
            {
                listen_for(DEF_PORT, 3);
            }
            else if (std::string(argv[0]) == "-p")
            {
                listen_for(std::stoi(argv[1]), 3);
            } else if (std::string(argv[0]) == "-e")
            {
                listen_for(DEF_PORT, std::stoi(argv[1]));
            } else {
                throw std::exception();
            }
            break;
        case 4:
            if (std::string(argv[0]) == "-p" && std::string(argv[2]) == "-e")
            {
                listen_for(std::stoi(argv[1]), std::stoi(argv[3]));
            }
            else if (std::string(argv[0]) == "-e" && std::string(argv[2]) == "-p")
            {
                listen_for(std::stoi(argv[3]), std::stoi(argv[1]));
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