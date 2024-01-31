#include <iostream>
#include <boost/asio.hpp>

#include "../include/message_creator.h"
#include "../include/constants.h"

using boost::asio::ip::tcp;

const std::string DEF_HOST = "localhost";

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

size_t send_message(tcp::socket &client_socket, msgCreators::Message_UPTR msg)
{
    std::string serialized_msg;
    bool did_serialize = msg->SerializeToString(&serialized_msg);

    std::cout << "serialize:" << did_serialize << std::endl;
    std::cout << "Serialized message as debug str:" << msg->DebugString() << std::endl;

    // size_t -> uint32 to make len fixed (size_t can be different)
    config::MESSAGE_SIZE msg_size = serialized_msg.size();

    auto bytes_sent = boost::asio::write(client_socket, boost::asio::buffer(&msg_size, sizeof(msg_size)));

    // Then, send the message itself
    bytes_sent += boost::asio::write(client_socket, boost::asio::buffer(serialized_msg));

    return bytes_sent;
}

tcp::socket make_socket(boost::asio::io_service &io_service,
                        std::string host = DEF_HOST,
                        std::string port = std::to_string(config::DEF_PORT))
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

msgCreators::Message_UPTR process_user_request(int choice, bool &loop, int &wait_for_reply)
{
    msgCreators::Message_UPTR msg(nullptr);
    switch (choice)
    {
    case 0:
        loop = false;
        break;
    case 1:
        msg = msgCreators::CommandConnect().create();
        wait_for_reply = 1;
        break;
    case 2:
        msg = msgCreators::CommandDisconnect().create();
        // wait for reply so read drops program
        wait_for_reply = 1;
        break;
    case 3:
        msg = msgCreators::CommandGetInfo().create();
        wait_for_reply = 1;
        break;
    case 4:
        msg = msgCreators::Pong().create();
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

        if(msg == nullptr)
            break;

        if (msg->IsInitialized())
        {
            size_t bytes_transfered = send_message(client_socket, std::move(msg));
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