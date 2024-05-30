#include <iostream>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;

void handle_request(http::request<http::string_body> req, http::response<http::string_body>& res) {
    res.version(req.version());
    res.result(http::status::ok);
    res.set(http::field::server, "Beast");
    res.set(http::field::content_type, "text/plain");
    res.body() = "Hello, World!";
    res.prepare_payload();
}

void session(asio::ip::tcp::socket socket) {
    try {
        beast::flat_buffer buffer;
        http::request<http::string_body> req;
        http::read(socket, buffer, req);

        http::response<http::string_body> res;
        handle_request(std::move(req), res);

        http::write(socket, res);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    try {
        asio::io_context io_context;
        asio::ip::tcp::acceptor acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 8080));

        while (true) {
            asio::ip::tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread(&session, std::move(socket)).detach();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}
