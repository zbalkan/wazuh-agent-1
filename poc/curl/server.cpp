#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/config.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <ctime>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

void fail(beast::error_code ec, char const* what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

struct TokenInfo {
    std::string token;
    std::time_t expiry;
};

std::unordered_map<std::string, TokenInfo> validTokens;

std::string generateToken() {
    std::string token = "new_token";
    return token;
}

bool verifyToken(const std::string& uuid, const std::string& token) {
    return true;
}

class session : public std::enable_shared_from_this<session> {
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;

public:
    explicit session(tcp::socket socket) : socket_(std::move(socket)) {}

    void run() {
        do_read();
    }

private:
    void do_read() {
        auto self = shared_from_this();
        http::async_read(socket_, buffer_, req_,
            [self](beast::error_code ec, std::size_t bytes_transferred) {
                boost::ignore_unused(bytes_transferred);
                if (!ec)
                    self->do_write();
                else
                    fail(ec, "read");
            });
    }

    void do_write() {
        res_.version(req_.version());
        res_.keep_alive(req_.keep_alive());
        res_.set(http::field::server, "Boost.Beast");
        res_.set(http::field::content_type, "text/plain");

        if (req_.method() == http::verb::get) {
            res_.result(http::status::ok);
            res_.body() = "Hello, World!";
        } else if (req_.method() == http::verb::post) {
            if (req_.target() == "/login") {
                handleLogin();
            } else {
                res_.result(http::status::ok);
                res_.body() = "Received: " + req_.body();
            }
        } else {
            res_.result(http::status::bad_request);
            res_.body() = "Invalid request method";
        }

        res_.prepare_payload();

        auto self = shared_from_this();
        http::async_write(socket_, res_,
            [self](beast::error_code ec, std::size_t bytes_transferred) {
                boost::ignore_unused(bytes_transferred);
                if (!ec) {
                    self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                } else {
                    fail(ec, "write");
                }
            });
    }

    void handleLogin() {
        auto body = req_.body();
        auto uuidPos = body.find("uuid=");
        auto tokenPos = body.find("token=");
        if (uuidPos != std::string::npos && tokenPos != std::string::npos) {
            std::string uuid = body.substr(uuidPos + 5, tokenPos - uuidPos - 6);
            std::cout << uuid << std::endl;
            std::string token = body.substr(tokenPos + 6);
            std::cout << token << std::endl;

            if (verifyToken(uuid, token)) {
                std::string newToken = generateToken();
                validTokens[uuid] = {newToken, std::time(nullptr) + 3600}; // 1 hour expiry

                res_.result(http::status::ok);
                res_.body() = "New Token: " + newToken;
            } else {
                res_.result(http::status::unauthorized);
                res_.body() = "Invalid or expired token";
            }
        } else {
            res_.result(http::status::bad_request);
            res_.body() = "Invalid request format";
        }
    }
};

class listener : public std::enable_shared_from_this<listener> {
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    listener(net::io_context& ioc, tcp::endpoint endpoint)
        : ioc_(ioc), acceptor_(net::make_strand(ioc)) {
        beast::error_code ec;

        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            fail(ec, "open");
            return;
        }

        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec) {
            fail(ec, "set_option");
            return;
        }

        acceptor_.bind(endpoint, ec);
        if (ec) {
            fail(ec, "bind");
            return;
        }

        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec) {
            fail(ec, "listen");
            return;
        }
    }

    void run() {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            net::make_strand(ioc_),
            [self = shared_from_this()](beast::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<session>(std::move(socket))->run();
                } else {
                    fail(ec, "accept");
                }
                self->do_accept();
            });
    }
};

int main(int argc, char* argv[]) {
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(8080);

    net::io_context ioc{ 1 };

    std::make_shared<listener>(ioc, tcp::endpoint{ address, port })->run();

    ioc.run();

    return 0;
}
