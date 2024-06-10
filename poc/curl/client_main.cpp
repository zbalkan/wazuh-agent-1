#include <iostream>
#include <string>

#include "client.hpp"
#include "defs.hpp"
#include "logger.hpp"
#include "token.hpp"
#include "unix_socket_handler.hpp"

int main()
{
    Client client(kURL, kUUID, kPASSWORD, session_token);

    try
    {
        boost::asio::io_context ioc;

        auto handler = std::make_shared<UnixSocketHandler>(client, ioc, "/tmp/command_socket");

        ioc.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    Logger::log("MAIN", "Main thread is exiting.");
    return 0;
}
