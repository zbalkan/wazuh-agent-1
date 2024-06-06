#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/config.hpp>
#include <boost/url.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <random>

#include "defs.hpp"
#include "token.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace urls = boost::urls;
using tcp = net::ip::tcp;

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
    void print_req() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        auto endpoint = socket_.remote_endpoint();
        std::string client_ip = endpoint.address().to_string();
        unsigned short client_port = endpoint.port();

        std::cout << "Timestamp: " << std::ctime(&now_time);
        std::cout << "Client IP: " << client_ip << ":" << client_port << std::endl;
        std::cout << "HTTP Request:" << std::endl;
        std::cout << req_ << std::endl;
    }

    void print_res() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        auto endpoint = socket_.remote_endpoint();
        std::string client_ip = endpoint.address().to_string();
        unsigned short client_port = endpoint.port();

        std::cout << "Timestamp: " << std::ctime(&now_time);
        std::cout << "Client IP: " << client_ip << ":" << client_port << std::endl;
        std::cout << "HTTP Response:" << std::endl;
        std::cout << res_ << std::endl;
    }

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
        std::cout << "==================== REQUEST START ====================\n";
        print_req();
        res_.version(req_.version());
        res_.keep_alive(req_.keep_alive());
        res_.set(http::field::server, "Boost.Beast");
        res_.set(http::field::content_type, "text/plain");

        if (req_.method() == http::verb::get) {
            urls::url_view url_view(req_.target());
            if (url_view.path() == "/commands") {
                handleCommands();
            } else {
                res_.result(http::status::ok);
                res_.body() = "Hello, World!";
            }
        } else if (req_.method() == http::verb::post) {
            if (req_.target() == "/login") {
                handleLogin();
            } else if (req_.target() == "/stateless") {
                handleStateless();
            } else if (req_.target() == "/commands") {
                res_.result(http::status::ok);
                res_.body() = "Received commands post request";
            } else {
                res_.result(http::status::ok);
                res_.body() = "Received: " + req_.body();
            }
        } else {
            res_.result(http::status::bad_request);
            res_.body() = "Invalid request method";
        }

        res_.prepare_payload();

        std::cout << "==================== REQUEST END ======================\n\n";
        std::cout << "==================== RESPONSE START ===================\n";
        print_res();
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
        std::cout << "==================== RESPONSE END =====================\n\n";
    }

    void handleLogin() {
        auto body = req_.body();
        auto uuidPos = body.find(uuidKey);
        auto passwordPos = body.find(passwordKey);
        if (uuidPos != std::string::npos && passwordPos != std::string::npos) {
            std::string uuid = body.substr(uuidPos + uuidKey.length(), passwordPos - uuidPos - uuidKey.length() - 1);
            std::string password = body.substr(passwordPos + passwordKey.length());

            if (verifyPassword(uuid, password)) {
                std::string newToken = createToken();
                validTokens[uuid] = {newToken, std::time(nullptr) + 3600}; // 1 hour expiry

                res_.result(http::status::ok);
                res_.body() = newToken;
            } else {
                res_.result(http::status::unauthorized);
                res_.body() = "Invalid or expired password";
            }
        } else {
            res_.result(http::status::bad_request);
            res_.body() = "Invalid request format";
        }
    }

    void handleStateless() {
        auto authHeader = req_["Authorization"];

        if (authHeader.empty() || authHeader.find(bearerPrefix) == std::string::npos) {
            res_.result(http::status::unauthorized);
            res_.body() = "Missing token";
            return;
        }

        std::string token = authHeader.substr(bearerPrefix.length());

        auto body = req_.body();
        auto uuidPos = body.find(uuidKey);
        auto eventPos = body.find(eventKey);
        if (uuidPos != std::string::npos) {
            std::string uuid = body.substr(uuidPos + uuidKey.length(), eventPos - uuidPos - uuidKey.length() - 1);

            if (token == validTokens[uuid].token && verifyToken(token)) {
                res_.result(http::status::ok);
                res_.body() = "Valid token";
            } else {
                res_.result(http::status::unauthorized);
                res_.body() = "Invalid or expired token";
            }
        } else {
            res_.result(http::status::bad_request);
            res_.body() = "Invalid request format";
        }
    }

    void handleCommands() {
        auto authHeader = req_["Authorization"];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 1);
        int randomValue = distrib(gen);

        if (authHeader.empty() || authHeader.find(bearerPrefix) == std::string::npos) {
            res_.result(http::status::unauthorized);
            res_.body() = "Missing token";
            return;
        }

        std::string token = authHeader.substr(bearerPrefix.length());
        urls::url_view url_view(req_.target());
        std::string uuid;

        if (url_view.has_query()) {
            for (auto param : url_view.params()) {
                if (param.key == "uuid") {
                    uuid = std::string(param.value);
                    break;
                }
            }
        }

        if (!uuid.empty()) {
            if (token == validTokens[uuid].token && verifyToken(token)) {
                if (randomValue == 0) {
                    std::this_thread::sleep_for(std::chrono::seconds(10));
                    res_.result(http::status::request_timeout);
                } else {
                    res_.result(http::status::ok);
                    res_.body() = "Command to run";
                }
            } else {
                res_.result(http::status::unauthorized);
                res_.body() = "Invalid or expired token";
            }
        } else {
            std::cout << "UUID not found in query parameters" << std::endl;
        }
    }
};
