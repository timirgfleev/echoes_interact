#include <iostream>
#include <boost/asio.hpp>
// #include "server.h"
#include "launcher.h"







int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    launcher(argc, argv);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}