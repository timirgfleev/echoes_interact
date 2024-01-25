#include "data.pb.h"
#include <iostream>
#include <boost/asio.hpp>


using boost::asio::ip::tcp;

void handle_connection(tcp::socket socket){
    //todo: use async read and timer to implement further functionality; 
}

//main loop
void listen(){
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



int main(int argc , char* argv[])
{
    coolProtocol::MessageWrapper messageWrapper;
    coolProtocol::HostCommand * hc = new coolProtocol::HostCommand();

    hc->set_command(coolProtocol::HostCommand::Command::HostCommand_Command_COMMAND_CONNECT);

    messageWrapper.set_allocated_host_command(hc);
    // tutorial::Person person;
    // person.set_name("zhangsan");
    // person.set_id(1);
    // person.set_email("123");


    std::cout << messageWrapper.IsInitialized();

    //delete hc;
}

