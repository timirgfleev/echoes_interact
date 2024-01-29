#pragma once
#include <cstdint>
namespace config
{
    // as name says
    const int DEFAULT_LOOP_COUNT = 3;
    const int DEF_PORT = 1234;

    // This type used as default timeout in seconds
    const int TIMEOT_LEN = 10;

    // This type describes size message prefix
    typedef uint32_t MESSAGE_SIZE;
}