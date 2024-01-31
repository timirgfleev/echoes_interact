## Description

This project is a server-client application that uses Google's Protocol Buffers for communication.


## Server description

### Event Sequence

1. Open endpoint on port.
2. Start main loop.
3. Listen for connections.
4. Receive a connection.
   - Start a new message exchange loop and hand over the ownership of the socket to them.
   - Run the message exchange loop on `boost::io_service`.
   - Break the message exchange loop (specifically, not continue).
5. Continue the main loop.
6. Check for main loop exit.

### Class Description

- `Server`: Operates the message exchange loop. It relies on the `MessageProcessor` member to interact with messages.
- `MessageProcessor`: Manages messages. It relies on `PermissionChecker` to manage user access for actions. It provides an interface to check if a message suggests a timeout and to check the status of a message action.

## Client description
- as simple as possible
- prints menu, reads digits to choose options
- Can close connection, also can be forcefully closed with ctrl+c 

## Getting Started

### Dependencies

- CMake (version 3.5 or higher)
- Google's Protocol Buffers
- Boost (version 1.58.0 or higher)

### Building

To build the project, run the following commands in the project root directory:

```sh
mkdir build
cd build
cmake ..
make
```

This will generate two executables: `info_service` and `info_client`.

### Running

To run the server, use the `info_service` executable. You can specify the port and the number of connections to accept before exiting:

```sh
./info_service -p <port> -e <number_of_connections>
```

If no options are provided, the server will run for 3 connections on port 1234:

```sh
./info_service -r
```

To run the client, use the `info_client` executable.

Exit codes (from `server.h`):

```cpp
    enum class ServerError
    {
        OK = 0,
        INTERNAL_ERROR = 1,
        INTERNAL_PROCESS_ERROR = 2,
        BAD_DATA = 3,
        BAD_BEHAVIOR = 4,
        TIMEOUT = 5,
        CONNECTION_CLOSE = 6,
        UNKNOWN_ERROR = 7,
    };
```

## Testing

The project includes tests for the launcher and the message processor classes. To run the tests, use the following command from build directory:

```sh
ctest
```

### valgrind test
```
==3416== HEAP SUMMARY:
==3416==     in use at exit: 0 bytes in 0 blocks
==3416==   total heap usage: 1,928 allocs, 1,928 frees, 257,678 bytes allocated
==3416== 
==3416== All heap blocks were freed -- no leaks are possible
==3416== 
==3416== For lists of detected and suppressed errors, rerun with: -s
==3416== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```

## Files
- `receiver/main.cpp`: Captures CLI args for launching

- `receiver/launcher.h`: Forwards CLI args to main_loop.
- `receiver/launcher.cpp`: Contains the implementation of the launcher function.

- `receiver/main_loop.h`: Contains the main loop (connection processing loop).
- `receiver/main_loop.cpp`: Defines the main loop of the application.

- `receiver/server.h`: Contains the declaration of the Server class.
- `receiver/server.cpp`: Contains the definition of the Server class.

- `receiver/message_processor.h`: Contains the declaration of the MessageProcessor class.
- `receiver/message_processor.h`: Contains the definition of the MessageProcessor class.

- `include/constants.h`: Contains constant values used throughout the project.
- `include/message_creator.h`: Contanins declaration for message factories.
- `include/message_creator.cpp`: Implements `message_creator.h` declarations.


- `host/host.cpp`: Contains the main function of the client.


- `test/cmd_args.cpp`: Contains tests for the launcher.
- `test/msg_process.cpp`: Contains tests for the message processor.
