#pragma once

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

#include "token.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
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
        auto passwordPos = body.find("password=");
        if (uuidPos != std::string::npos && passwordPos != std::string::npos) {
            std::string uuid = body.substr(uuidPos + 5, passwordPos - uuidPos - 6);
            std::cout << uuid << std::endl;
            std::string password = body.substr(passwordPos + 6);
            std::cout << password << std::endl;

            if (verifyPassword(uuid, password)) {
                std::string newToken = generateToken();
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
};
