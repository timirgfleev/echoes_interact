#include <iostream>
#include <boost/asio.hpp>
#include "../receiver/server.h"

using boost::asio::ip::tcp;

coolProtocol::MessageWrapper make_pong_message(){
    std::cout << "pong" << std::endl;

    coolProtocol::MessageWrapper msg;
    coolProtocol::Pong *pong = new coolProtocol::Pong();
    msg.set_allocated_pong(pong);

    return std::move(msg);
}

coolProtocol::MessageWrapper make_ping_message(){
    std::cout << "ping" << std::endl;

    coolProtocol::MessageWrapper msg;
    coolProtocol::Ping *ping = new coolProtocol::Ping();
    msg.set_allocated_ping(ping);

    return std::move(msg);
}

coolProtocol::MessageWrapper make_host_command(coolProtocol::HostCommand::Command command){
    std::cout << "host command" << std::endl;

    coolProtocol::MessageWrapper msg;
    coolProtocol::HostCommand *host_command = new coolProtocol::HostCommand();
    host_command->set_command(command);
    msg.set_allocated_request(host_command);

    return std::move(msg);
}

coolProtocol::MessageWrapper get_message(tcp::socket& client_socket){
    boost::asio::streambuf response_buffer;
    std::size_t bytes_transferred = boost::asio::read_until(client_socket, response_buffer, '\0'); // Read until newline character.

    std::istream is(&response_buffer);
    std::string response_string(bytes_transferred, 0); // Subtract 1 because read_until includes the delimiter in the count
    is.read(&response_string[0], bytes_transferred);

    coolProtocol::MessageWrapper response;
    bool parsing_done = response.ParseFromString(response_string);

    if (!parsing_done)
        throw std::runtime_error("Server send weird message");

    return std::move(response);
}

void send_message(tcp::socket& client_socket, coolProtocol::MessageWrapper& msg){
    std::string serialized_msg;
    bool did_serialize = msg.SerializeToString(&serialized_msg);
    std::cout << "serialize:" << did_serialize << std::endl;
    // serialized_msg += '\n';
    std::cout << "Serialized message: " << serialized_msg << std::endl;

    serialized_msg += '\0';

    boost::asio::write(client_socket, boost::asio::buffer(serialized_msg));
}

int main(int argc, char *argv[])
{
    // GOOGLE_PROTOBUF_VERIFY_VERSION;

    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("localhost", "1234");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket client_socket(io_service);
    boost::asio::connect(client_socket, endpoint_iterator);


    coolProtocol::MessageWrapper msg;

    coolProtocol::HostCommand *hc = new coolProtocol::HostCommand();
    hc->set_command(coolProtocol::HostCommand::COMMAND_CONNECT);
    msg.set_allocated_request(hc);

    send_message(client_socket, msg);

    coolProtocol::MessageWrapper response = get_message(client_socket);


    coolProtocol::MessageWrapper expected;
    coolProtocol::Ping ping;
    expected.set_allocated_ping(&ping);


    assert(response.DebugString() == expected.DebugString());
    std::cout << "assertion passed" << std::endl;
    //google::protobuf::ShutdownProtobufLibrary();
}