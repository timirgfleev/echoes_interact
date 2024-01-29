#include <iostream>
#include <boost/asio.hpp>
// #include "server.h"
#include "launcher.h"

using boost::asio::ip::tcp;





int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    launcher(argc, argv);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}