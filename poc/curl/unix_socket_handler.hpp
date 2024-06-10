#pragma once

#include <boost/asio.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <iostream>
#include <memory>
#include <sys/un.h>
#include <unistd.h>

#include "client.hpp"
#include "defs.hpp"
#include "event_queue_monitor.hpp"
#include "logger.hpp"
#include "requests.hpp"
#include "token.hpp"

class UnixSocketHandler : public std::enable_shared_from_this<UnixSocketHandler>
{
public:
    UnixSocketHandler(Client& client, boost::asio::io_context& ioc, const std::string& socket_path)
        : client_(client)
        , ioc_(ioc)
        , socket_(ioc)
        , acceptor_(ioc)
    {
        unlink(socket_path.c_str());

        boost::system::error_code ec;
        boost::asio::local::stream_protocol::endpoint ep(socket_path);
        acceptor_.open(ep.protocol(), ec);
        if (ec)
        {
            std::cerr << "Failed to open acceptor: " << ec.message() << "\n";
            return;
        }

        acceptor_.bind(ep, ec);
        if (ec)
        {
            std::cerr << "Failed to bind acceptor: " << ec.message() << "\n";
            return;
        }

        acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec)
        {
            std::cerr << "Failed to listen: " << ec.message() << "\n";
            return;
        }

        start();
    }

    void start()
    {
        do_accept();
    }

    void stop()
    {
        boost::system::error_code ec;
        acceptor_.close(ec);
        if (ec)
        {
            std::cerr << "Error closing acceptor: " << ec.message() << "\n";
        }
        ioc_.stop();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, boost::asio::local::stream_protocol::socket socket)
            {
                if (!ec)
                {
                    auto socket_ptr = std::make_shared<boost::asio::local::stream_protocol::socket>(std::move(socket));
                    handle_client(socket_ptr);
                }
                else
                {
                    std::cerr << "Error on accept: " << ec.message() << "\n";
                }

                do_accept();
            });
    }

    void handle_client(std::shared_ptr<boost::asio::local::stream_protocol::socket> socket)
    {
        auto buffer = std::make_shared<boost::asio::streambuf>();

        auto read_handler = [this, socket, buffer](boost::system::error_code ec, std::size_t length)
        {
            if (!ec)
            {
                std::istream input_stream(buffer.get());
                std::string command;
                std::getline(input_stream, command);
                Logger::log("SOCKET HANDLER", "Received command: " + command);

                std::string response = "Processed: " + command + "\n";
                boost::asio::async_write(
                    *socket,
                    boost::asio::buffer(response),
                    [this, socket, buffer, command](boost::system::error_code ec, std::size_t)
                    {
                        if (!ec)
                        {
                            if (command == "exit")
                            {
                                stop();
                                return;
                            }

                            handle_client(socket);
                        }
                        else
                        {
                            std::cerr << "Error on write: " << ec.message() << "\n";
                        }
                    });

                if (command == "login")
                {
                    SendLoginRequest(kURL, kUUID, kPASSWORD, session_token);
                }
                else if (command == "stateless")
                {
                    SendStatelessRequest(kURL, kUUID, session_token, "");
                }
                else if (command == "stopcommands")
                {
                    client_.commandDispatcher->keepCommandDispatcherRunning.store(false);
                }
                else if (command == "get")
                {
                    SendGetRequest(kURL);
                }
                else if (command == "post")
                {
                    std::string postData = "Hello, this is a POST request.";
                    SendPostRequest(kURL, postData);
                }
                else if (command == "cleartoken")
                {
                    session_token.clear();
                }
                else if (command == "createevent")
                {
                    static int event = 0;
                    client_.eventQueueMonitor->eventQueue->insertEvent(event++, "{\"key\": \"value\"}", "json");
                }
                else
                {
                    Logger::log("SOCKET HANDLER", "Available commands: login, stateless, stopcommands, get, post, "
                                                 "cleartoken, createevent, exit");
                }
            }
            else
            {
                std::cerr << "Error on read: " << ec.message() << "\n";
            }
        };

        boost::asio::async_read_until(*socket, *buffer, '\n', read_handler);
    }

    Client& client_;
    boost::asio::io_context& ioc_;
    boost::asio::local::stream_protocol::socket socket_;
    boost::asio::local::stream_protocol::acceptor acceptor_;
};
