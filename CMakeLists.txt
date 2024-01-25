cmake_minimum_required(VERSION 3.5)
project(info_service VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

find_package(Protobuf REQUIRED)
include_directories(${PROTOBUF_INCLUDE_DIRS})

find_package(Boost 1.58.0 REQUIRED COMPONENTS system)
include_directories(${Boost_INCLUDE_DIRS})


include_directories(${CMAKE_CURRENT_BINARY_DIR})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS proto/data.proto)

add_executable(info_service receiver/main.cpp ${PROTO_SRCS} ${PROTO_HDRS})

target_link_libraries(info_service ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)