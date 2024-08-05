#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/config.hpp>
#include <boost/url.hpp>
#include <chrono>
#include <ctime>
#include <iostream>
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "defs.hpp"
#include "token.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
namespace urls = boost::urls;
using tcp = net::ip::tcp;
using json = nlohmann::json;

class session : public std::enable_shared_from_this<session>
{
    tcp::socket socket_;
    beast::flat_buffer buffer_;
    http::request<http::string_body> req_;
    http::response<http::string_body> res_;

public:
    explicit session(tcp::socket socket)
        : socket_(std::move(socket))
    {
    }

    void run()
    {
        do_read();
    }

private:
    void print_req()
    {
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

    void print_res()
    {
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

    void do_read()
    {
        auto self = shared_from_this();
        http::async_read(socket_,
                         buffer_,
                         req_,
                         [self](beast::error_code ec, std::size_t bytes_transferred)
                         {
                             boost::ignore_unused(bytes_transferred);
                             if (!ec)
                                 self->do_write();
                             else
                                 fail(ec, "read");
                         });
    }

    void do_write()
    {
        std::cout << "==================== REQUEST START ====================\n";
        print_req();
        res_.version(req_.version());
        res_.keep_alive(req_.keep_alive());
        res_.set(http::field::server, "Boost.Beast");
        res_.set(http::field::content_type, "json");

        if (req_.method() == http::verb::get)
        {
            urls::url_view url_view(req_.target());
            if (url_view.path() == "/commands")
            {
                handleCommands();
            }
            else
            {
                res_.result(http::status::ok);
                res_.body() = "Hello, World!";
            }
        }
        else if (req_.method() == http::verb::post)
        {
            if (req_.target() == "/authenticate")
            {
                handleServerLogin();
            }
            else if (req_.target() == "/authentication")
            {
                handleLogin();
            }
            else if (req_.target() == "/stateless")
            {
                handleStateless();
            }
            else if (req_.target() == "/commands")
            {
                res_.result(http::status::ok);
                res_.body() = "Received commands post request";
            }
            else if (req_.target() == "/agents")
            {
                handleRegister();
            }
            else
            {
                res_.result(http::status::ok);
                res_.body() = "Received: " + req_.body();
            }
        }
        else
        {
            res_.result(http::status::bad_request);
            res_.body() = "Invalid request method";
        }

        res_.prepare_payload();

        std::cout << "==================== REQUEST END ======================\n\n";
        std::cout << "==================== RESPONSE START ===================\n";
        print_res();
        auto self = shared_from_this();
        http::async_write(socket_,
                          res_,
                          [self](beast::error_code ec, std::size_t bytes_transferred)
                          {
                              boost::ignore_unused(bytes_transferred);
                              if (!ec)
                              {
                                  self->socket_.shutdown(tcp::socket::shutdown_send, ec);
                              }
                              else
                              {
                                  fail(ec, "write");
                              }
                          });
        std::cout << "==================== RESPONSE END =====================\n\n";
    }

    void handleServerLogin()
    {
        auto authHeader = req_["Authorization"];
        std::string user_pass;
        std::string user;
        std::string password;
        bool validRequest = true;

        if (authHeader.empty() || authHeader.find("Basic ") != 0)
        {
            res_.result(http::status::unauthorized);
            res_.body() = "Authorization header missing or incorrect";
            return;
        }

        // Extrae y decodifica el valor de Authorization
        user_pass = authHeader.substr(6);
        std::size_t colonPos = user_pass.find(':');

        if (colonPos == std::string::npos)
        {
            validRequest = false;
        }
        else
        {
            user = user_pass.substr(0, colonPos);
            password = user_pass.substr(colonPos + 1);
        }

        if (user.empty() || password.empty())
        {
            validRequest = false;
        }
        else
        {
            if (verifyPassword(user, password))
            {
                std::string newToken = createToken();
                validTokens[newToken] = {newToken, std::time(nullptr) + 3600}; // 1 hour expiry

                res_.result(http::status::ok);
                res_.body() = newToken;
            }
            else
            {
                res_.result(http::status::unauthorized);
                res_.body() = "Invalid or expired password";
            }
        }

        if (!validRequest)
        {
            res_.result(http::status::bad_request);
            res_.body() = "Invalid request format";
            return;
        }
    }

    void handleLogin()
    {
        json body;
        std::string uuid;
        bool validRequest = true;

        try
        {
            body = json::parse(req_.body());

            if (body.contains(uuidKey) && body.at(uuidKey).is_string())
            {
                uuid = body[uuidKey].get<std::string>();
            }
            else
            {
                validRequest = false;
            }

            if (!validRequest)
            {
                res_.result(http::status::bad_request);
                res_.body() = "Invalid request format";
                return;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing JSON body: " << e.what() << std::endl;
        }

        if (verifyUuid(uuid))
        {
            std::string newToken = createToken();
            validTokens[newToken] = {newToken, std::time(nullptr) + 3600}; // 1 hour expiry

            res_.result(http::status::ok);
            res_.body() = newToken;
        }
        else
        {
            res_.result(http::status::unauthorized);
            res_.body() = "Invalid uuid";
        }
    }

    void handleStateless()
    {
        auto authHeader = req_["Authorization"];

        if (authHeader.empty() || authHeader.find(bearerPrefix) == std::string::npos)
        {
            res_.result(http::status::unauthorized);
            res_.body() = "Missing token";
            return;
        }

        std::string token = authHeader.substr(bearerPrefix.length());
        std::string uuid;
        json events;
        json body;
        bool validRequest = true;

        try
        {
            body = json::parse(req_.body());

            if (body.contains(uuidKey) && body.at(uuidKey).is_string())
            {
                uuid = body[uuidKey].get<std::string>();
            }
            else
            {
                validRequest = false;
            }

            if (body.contains(eventKey) && body[eventKey].is_object() && body[eventKey].contains(eventsKey) &&
                body[eventKey][eventsKey].is_array())
            {
                events = body[eventKey][eventsKey].dump();
            }
            else
            {
                validRequest = false;
            }

            if (!validRequest)
            {
                res_.result(http::status::bad_request);
                res_.body() = "Invalid request format";
                return;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing JSON body: " << e.what() << std::endl;
        }

        if (token == validTokens[uuid].token && verifyToken(token))
        {

            try
            {
                std::cout << "\nParsed JSON Data:" << std::endl;
                std::cout << events << std::endl;
            }
            catch (json::parse_error& e)
            {
                std::cerr << "Failed to parse JSON: " << e.what() << std::endl;
            }

            res_.result(http::status::ok);
            res_.body() = "Stateless post request received";
        }
        else
        {
            res_.result(http::status::unauthorized);
            res_.body() = "Invalid or expired token";
        }
    }

    void handleCommands()
    {
        auto authHeader = req_["Authorization"];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 1);
        int randomValue = distrib(gen);

        if (authHeader.empty() || authHeader.find(bearerPrefix) == std::string::npos)
        {
            res_.result(http::status::unauthorized);
            res_.body() = "Missing token";
            return;
        }

        std::string token = authHeader.substr(bearerPrefix.length());
        urls::url_view url_view(req_.target());

        if (token == validTokens[token].token && verifyToken(token))
        {
            if (randomValue == 0)
            {
                std::this_thread::sleep_for(std::chrono::seconds(10));
                res_.result(http::status::request_timeout);
            }
            else
            {
                res_.result(http::status::ok);
                res_.body() = "{\"commands\":[{\"origin\":{\"module\":\"upgrade_module\"}, "
                                "\"command\":\"upgrade_update_status\","
                                "\"parameters\":{\"agents\":[20],\"error\":0,\"data\":\"Upgrade "
                                "Successful\",\"status\":\"Done\"}},"
                                "{\"origin\":{\"module\":\"upgrade_module\"},\"command\":\"upgrade_update_status\","
                                "\"parameters\":{\"agents\":[20],"
                                "\"error\":0,\"data\":\"Upgrade Successful\",\"status\":\"Done\"}}]}";
            }
        }
        else
        {
            res_.result(http::status::unauthorized);
            res_.body() = "Invalid or expired token";
        }
    }

    void handleRegister()
    {
        auto authHeader = req_["Authorization"];

        if (authHeader.empty() || authHeader.find(bearerPrefix) == std::string::npos)
        {
            res_.result(http::status::unauthorized);
            res_.body() = "Missing token";
            return;
        }

        std::string token = authHeader.substr(bearerPrefix.length());
        std::string uuid;
        std::string name;
        json body;
        bool validRequest = true;
        try
        {
            body = json::parse(req_.body());

            if (body.contains(uuidKey) && body.at(uuidKey).is_string())
            {
                uuid = body[uuidKey].get<std::string>();
            }
            else
            {
                validRequest = false;
            }

            if (body.contains(nameKey) && body.at(nameKey).is_string())
            {
                name = body[nameKey].get<std::string>();
            }

            if (!validRequest)
            {
                res_.result(http::status::bad_request);
                res_.body() = "Invalid request format";
                return;
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing JSON body: " << e.what() << std::endl;
        }

        if (token == validTokens[token].token && verifyToken(token))
        {
            res_.result(http::status::ok);
        }
        else
        {
            res_.result(http::status::unauthorized);
            res_.body() = "Invalid or expired token";
        }
    }
};
