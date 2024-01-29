#include "launcher.h"
#include "../include/constants.h"

// my eyes are in pain so this is autotested :)
void launcher(const int argc, const char *argv[])
{
    try
    {
        switch (argc)
        {
        case 2:
            if (std::string(argv[1]) == "-r")
            {
                listen_for(user_constant::DEF_PORT, user_constant::DEFAULT_LOOP_COUNT);
            }
            break;
        case 3:
            if (std::string(argv[1]) == "-p")
            {
                listen_for(std::stoi(argv[2]), user_constant::DEFAULT_LOOP_COUNT);
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
                  << "[if none optional selected] -r run for " << user_constant::DEFAULT_LOOP_COUNT
                  << " connections and on port" << user_constant::DEF_PORT << std::endl;
    }
}