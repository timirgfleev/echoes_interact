#include "launcher.h"
#include "data.pb.h"






int main(int argc, char *argv[])
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    launcher(argc, argv);

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}