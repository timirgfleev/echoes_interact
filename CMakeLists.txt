cmake_minimum_required(VERSION 3.5)
project(info_service VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_BUILD_TYPE Debug)

include(CTest)
enable_testing()

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(Boost 1.58.0 REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})


include_directories(${CMAKE_CURRENT_BINARY_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS proto/messages.proto)

add_executable(info_service 
                receiver/main.cpp 
                receiver/launcher.cpp 
                receiver/main_loop.cpp 
                receiver/server.cpp 
                receiver/message_processer.cpp 
                include/message_creator.cpp 
                ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(info_service ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)


add_executable(info_client 
                host/host.cpp 
                include/message_creator.cpp 
                ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(info_client ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

# Tests
add_executable(LauncherTest test/cmd_args.cpp receiver/launcher.cpp)
add_test(NAME LauncherTest COMMAND LauncherTest)

add_executable(MessageProcessorTest test/msg_process.cpp 
                                            receiver/message_processer.cpp include/message_creator.cpp 
                                            ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(MessageProcessorTest ${PROTOBUF_LIBRARIES})
add_test(NAME MessageProcessorTest COMMAND MessageProcessorTest)
