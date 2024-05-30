#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main() {
    try {
        net::io_context ioc;
        tcp::resolver resolver{ioc};
        auto const results = resolver.resolve("localhost", "8080");

        websocket::stream<tcp::socket> ws{ioc};
        net::connect(ws.next_layer(), results.begin(), results.end());

        ws.handshake("localhost", "/");

        std::string message = "Hello, WebSocket!";
        ws.write(net::buffer(std::string(message)));

        beast::flat_buffer buffer;
        ws.read(buffer);

        std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;

        // Keep the connection open to send and receive more messages
        for (;;) {
            std::getline(std::cin, message);
            ws.write(net::buffer(std::string(message)));
            ws.read(buffer);
            std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;
        }
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
