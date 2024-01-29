// message_processor_test.cpp
#include <cassert>
#include "../receiver/message_processor.h"
#include "data.pb.h"

// standard behaviour
void connect_and_get_data()
{
    //start consditions
    MessageProcesser processor;

    assert(processor.get_state() == MessageProcesser::ProcessingState::SUCCESS);
    assert(processor.is_deadline_set() == false);

    coolProtocol::MessageWrapper host_msg;
    auto request = new coolProtocol::HostCommand();
    request->set_command(coolProtocol::HostCommand::Command::HostCommand_Command_COMMAND_CONNECT);
    host_msg.set_allocated_request(request);

    auto response = processor.handle_message(std::move(host_msg));
    
    // has send ping request
    assert(response.get()->has_ping());
    assert(processor.get_state() == MessageProcesser::ProcessingState::SUCCESS);
    assert(processor.is_deadline_set() == true);

    host_msg.Clear();
    host_msg.set_allocated_pong(new coolProtocol::Pong);

    response = processor.handle_message(host_msg);

    // has accepted pong
    assert(response == nullptr);
    assert(processor.get_state() == MessageProcesser::ProcessingState::SUCCESS);
    assert(processor.is_deadline_set() == false);

    host_msg.Clear();
    request = new coolProtocol::HostCommand;
    request->set_command(coolProtocol::HostCommand::Command::HostCommand_Command_COMMAND_GET_DEVICE_INFO);
    host_msg.set_allocated_request(request);

    response = processor.handle_message(host_msg);

    // can send get_data now
    assert(response->has_device_data());
    assert(processor.get_state() == MessageProcesser::ProcessingState::SUCCESS);
    assert(processor.is_deadline_set() == false);
    host_msg.Clear();

    request = new coolProtocol::HostCommand();
    request->set_command(coolProtocol::HostCommand::Command::HostCommand_Command_COMMAND_DISCONNECT);
    host_msg.set_allocated_request(request);

    response = processor.handle_message(std::move(host_msg));

    // ok close connection
    assert(response == nullptr);
    assert(processor.get_state() == MessageProcesser::ProcessingState::CONNECTION_CLOSE);
    assert(processor.is_deadline_set() == false);
}

// instant disconnect command (I decide everyone must be able to do it)
void instant_disconnect()
{
    MessageProcesser processor;

    assert(processor.get_state() == MessageProcesser::ProcessingState::SUCCESS);
    assert(processor.is_deadline_set() == false);

    coolProtocol::MessageWrapper host_msg;
    auto request = new coolProtocol::HostCommand();
    request->set_command(coolProtocol::HostCommand::Command::HostCommand_Command_COMMAND_DISCONNECT);
    host_msg.set_allocated_request(request);

    auto response = processor.handle_message(std::move(host_msg));

    assert(response == nullptr);
    assert(processor.get_state() == MessageProcesser::ProcessingState::CONNECTION_CLOSE);
    assert(processor.is_deadline_set() == false);
}

// send just get data (server must reject)
void acess_violation()
{
    MessageProcesser processor;

    assert(processor.get_state() == MessageProcesser::ProcessingState::SUCCESS);
    assert(processor.is_deadline_set() == false);

    coolProtocol::MessageWrapper host_msg;
    auto request = new coolProtocol::HostCommand();
    request->set_command(coolProtocol::HostCommand::Command::HostCommand_Command_COMMAND_GET_DEVICE_INFO);
    host_msg.set_allocated_request(request);

    auto response = processor.handle_message(std::move(host_msg));

    assert(response == nullptr);
    assert(processor.get_state() == MessageProcesser::ProcessingState::PERMISSION_DENIED);
    assert(processor.is_deadline_set() == false);
}

int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    connect_and_get_data();
    instant_disconnect();
    acess_violation();
    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}