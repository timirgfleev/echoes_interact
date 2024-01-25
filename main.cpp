#include "data.pb.h"
#include <iostream>
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

