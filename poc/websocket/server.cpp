#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <thread>

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void handle_session(tcp::socket socket) {
    try {
        websocket::stream<tcp::socket> ws{std::move(socket)};
        ws.accept();

        for (;;) {
            beast::flat_buffer buffer;
            ws.read(buffer);
            std::cout << "Received: " << beast::make_printable(buffer.data()) << std::endl;

            ws.text(ws.got_text());
            ws.write(buffer.data());
        }
    } catch (beast::system_error const& se) {
        if (se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        net::io_context ioc{1};
        tcp::acceptor acceptor{ioc, tcp::endpoint{tcp::v4(), 8080}};

        for (;;) {
            tcp::socket socket{ioc};
            acceptor.accept(socket);
            std::thread{[s = std::move(socket)]() mutable { handle_session(std::move(s)); }}.detach();
        }
    } catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
