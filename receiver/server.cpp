#include "server.h"

Server::Server(tcp::socket sk)
    : sk_(std::move(sk)),
      buffer_(),
      next_message_size_(0),
      timer_(sk_.get_executor()),
      is_deadline_set_(false),
      msg_processer_(),
      state_(ServerError::OK)
{
}

Server::~Server()
{
    if (is_deadline_set_)
    {
        timer_.cancel();
    }
    std::cout << "Server destructor" << std::endl;
}

void Server::StartReceive()
{
    // std::cout << "Server start" << std::endl;

    // Start by reading the size of the next message.
    boost::asio::async_read(sk_, boost::asio::buffer(&next_message_size_, sizeof(next_message_size_)),
                            std::bind(&Server::size_received_callback, this,
                                      std::placeholders::_1, std::placeholders::_2));

    std::cout << "Server start listen" << std::endl;
}

void Server::size_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    std::cout << "Server size received callback" << std::endl;
    if (!error)
    {

        // Now that we know the size of the next message, read the message itself.
        boost::asio::async_read(sk_, buffer_, boost::asio::transfer_exactly(next_message_size_),
                                std::bind(&Server::message_received_callback, this,
                                          std::placeholders::_1, std::placeholders::_2));
    }
    else
    {
        state_ = ServerError::INTERNAL_ERROR;
    }
}

void Server::message_received_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{

    if (is_deadline_set_)
    {
        std::cout << "Server timer canceled" << std::endl;
        timer_.cancel();
    }

    std::cout << "Server received " << bytes_transferred << "bytes" << std::endl;
    if (!error)
    {
        std::istream is(&buffer_);
        coolProtocol::MessageWrapper msg;

        bool parsing_done = msg.ParseFromIstream(&is);

        buffer_.consume(buffer_.size());

        if (parsing_done)
        {
            std::cout << "Parsing done: " << parsing_done << std::endl;
            auto result = msg_processer_.handle_message(std::move(msg));

            auto error = msg_processer_.get_state();

            if (!is_continue_state(error))
            {
                std::cout << "Server error: " << static_cast<int>(error) << std::endl;
                close_connection();
                return;
            }

            if (msg_processer_.is_deadline_set())
            {
                wait_for_reply();
            }

            if (result)
            {
                send_reply(std::move(*result));
            }
            else
            {
                StartReceive();
            }
        }
        else
        {
            state_ = ServerError::BAD_DATA;
        }
    }
    else
    {
        state_ = ServerError::INTERNAL_ERROR;
    }
}

void Server::send_callback(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    std::cout << "Server send callback" << std::endl;
    std::cout << "Server sent " << bytes_transferred << "bytes" << std::endl;
    if (!error)
    {
        StartReceive();
    }
    else
    {
        state_ = ServerError::INTERNAL_ERROR;
    }
}

bool Server::is_continue_state(MessageProcesser::ProcessingState error)
{
    bool res = false;
    switch (error)
    {
    case MessageProcesser::ProcessingState::SUCCESS:
        state_ = ServerError::OK;
        res = true;
        break;
    case MessageProcesser::ProcessingState::PERMISSION_DENIED:
        state_ = ServerError::BAD_BEHAVIOR;
        res = false;
        break;
    case MessageProcesser::ProcessingState::UNKNOWN_MESSAGE:
        state_ = ServerError::BAD_DATA;
        res = false;
        break;
    case MessageProcesser::ProcessingState::INTERNAL_ERROR:
        state_ = ServerError::INTERNAL_PROCESS_ERROR;
        res = false;
        break;
    case MessageProcesser::ProcessingState::CONNECTION_CLOSE:
        // user wants to close connection... ok
        state_ = ServerError::OK;
        res = false;
        break;

    default:
        state_ = ServerError::UNKNOWN_ERROR;
        res = false;
        break;
    }
    return res;
}

void Server::send_reply(coolProtocol::MessageWrapper reply_msg)
{
    std::ostringstream stream;
    bool did_serialize = reply_msg.SerializeToOstream(&stream);

    std::cout << "Server sent:" << reply_msg.DebugString() << std::endl;

    if (did_serialize)
    {
        std::string serialized_msg = stream.str();
        uint32_t msg_size = serialized_msg.size();

        std::vector<boost::asio::const_buffer> buffers;
        buffers.push_back(boost::asio::buffer(&msg_size, sizeof(msg_size)));
        buffers.push_back(boost::asio::buffer(serialized_msg));

        boost::asio::async_write(sk_, buffers,
                                 std::bind(&Server::send_callback, this, std::placeholders::_1, std::placeholders::_2));
    }
    else
    {
        state_ = ServerError::INTERNAL_ERROR;
    }
}

void Server::close_connection()
{
    std::cout << "Server conn close" << std::endl;
    sk_.close();
}

void Server::wait_for_reply(u_short deadline)
{
    std::cout << "Server timer armed" << std::endl;
    is_deadline_set_ = true;
    timer_.expires_from_now(boost::posix_time::seconds(deadline));

    timer_.async_wait([this](const boost::system::error_code &error)
                      { if (!error && sk_.is_open()) {
                                this->on_timeout();
                            } });
}

void Server::on_timeout()
{
    state_ = ServerError::TIMEOUT;
    close_connection();
}

