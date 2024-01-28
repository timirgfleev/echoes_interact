#include <iostream>
#include <boost/asio.hpp>

#include "data.pb.h"
#include "../include/constants.h"

using boost::asio::ip::tcp;

coolProtocol::MessageWrapper make_pong_message()
{
    std::cout << "pong" << std::endl;

    coolProtocol::MessageWrapper msg;
    coolProtocol::Pong *pong = new coolProtocol::Pong();
    msg.set_allocated_pong(pong);

    return std::move(msg);
}

coolProtocol::MessageWrapper make_ping_message()
{
    std::cout << "ping" << std::endl;

    coolProtocol::MessageWrapper msg;
    coolProtocol::Ping *ping = new coolProtocol::Ping();
    msg.set_allocated_ping(ping);

    return std::move(msg);
}

coolProtocol::MessageWrapper make_host_command(coolProtocol::HostCommand::Command command)
{
    std::cout << "host command" << std::endl;

    coolProtocol::MessageWrapper msg;
    coolProtocol::HostCommand *host_command = new coolProtocol::HostCommand();
    host_command->set_command(command);
    msg.set_allocated_request(host_command);

    return std::move(msg);
}

coolProtocol::MessageWrapper listen_to_message(tcp::socket &client_socket)
{
    uint32_t msg_size;
    boost::asio::read(client_socket, boost::asio::buffer(&msg_size, sizeof(msg_size)));

    std::vector<char> response_buffer(msg_size);
    boost::asio::read(client_socket, boost::asio::buffer(response_buffer));

    std::cout << "Received " << msg_size << " bytes" << std::endl;

    coolProtocol::MessageWrapper response;
    bool parsing_done = response.ParseFromArray(response_buffer.data(), msg_size);

    if (!parsing_done)
        throw std::runtime_error("Server send weird message");

    return std::move(response);
}

size_t send_message(tcp::socket &client_socket, coolProtocol::MessageWrapper &msg)
{
    std::string serialized_msg;
    bool did_serialize = msg.SerializeToString(&serialized_msg);

    std::cout << "serialize:" << did_serialize << std::endl;
    std::cout << "Serialized message as debug str:" << msg.DebugString() << std::endl;

    // size_t -> uint32 to make len fixed (size_t can be different)
    uint32_t msg_size = serialized_msg.size();

    auto bytes_sent = boost::asio::write(client_socket, boost::asio::buffer(&msg_size, sizeof(msg_size)));

    // Then, send the message itself
    bytes_sent+= boost::asio::write(client_socket, boost::asio::buffer(serialized_msg));

    return bytes_sent;
}

tcp::socket make_socket(boost::asio::io_service &io_service,
                        std::string host = "localhost",
                        std::string port = "1234")
{
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, port);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket client_socket(io_service);
    boost::asio::connect(client_socket, endpoint_iterator);

    return std::move(client_socket);
}

void printMenu()
{
    std::cout << "Select a command: \n"
              << "0. Exit\n"
              << "1. Connect\n"
              << "2. Disconnect\n"
              << "3. Get device data\n"
              << "4. Send pong"
              << std::endl;
}

coolProtocol::MessageWrapper process_user_request(int choice, bool &loop, int &wait_for_reply)
{
    coolProtocol::MessageWrapper msg;
    switch (choice)
    {
    case 0:
        loop = false;
        break;
    case 1:
        msg = make_host_command(coolProtocol::HostCommand::COMMAND_CONNECT);
        wait_for_reply = 1;
        break;
    case 2:
        msg = make_host_command(coolProtocol::HostCommand::COMMAND_DISCONNECT);
        //wait for reply so read drops program
        wait_for_reply = 1;
        break;
    case 3:
        msg = make_host_command(coolProtocol::HostCommand::COMMAND_GET_DEVICE_INFO);
        wait_for_reply = 1;
        break;
    case 4:
        msg = make_pong_message();
        wait_for_reply = 0;
        break;
    default:
        std::cout << "Invalid choice" << std::endl;
        wait_for_reply = 0;
        break;
    }
    return std::move(msg);
}

void chat(tcp::socket &client_socket)
{
    bool loop = true;
    while (loop)
    {

        printMenu();
        int choice;
        std::cin >> choice;

        int wait_for_reply;
        auto msg = process_user_request(choice, loop, wait_for_reply);
        if (msg.IsInitialized())
        {
            size_t bytes_transfered = send_message(client_socket, msg);
            std::cout << "Sent " << bytes_transfered << " bytes" << std::endl;
            if (wait_for_reply)
            {
                auto response = listen_to_message(client_socket);
                std::cout << "Get message:" << response.DebugString() << std::endl;
            }
        }
        else if (!client_socket.is_open())
        {
            std::cout << "Connection closed, exit...." << std::endl;
            loop = false;
        }
    }
}

int main(int argc, char *argv[])
{
    // GOOGLE_PROTOBUF_VERIFY_VERSION;

    boost::asio::io_service io_service;

    auto client_socket = make_socket(io_service);

    try
    {
        chat(client_socket);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}