#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

int main() {
    try {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("localhost", "8080");
        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        http::request<http::string_body> req{http::verb::get, "/", 11};
        req.set(http::field::host, "localhost");
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        http::write(socket, req);

        beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;
        http::read(socket, buffer, res);

        std::cout << res << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
