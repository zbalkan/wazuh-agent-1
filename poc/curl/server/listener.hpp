#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/config.hpp>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "defs.hpp"
#include "session.hpp"
#include "token.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

class listener : public std::enable_shared_from_this<listener>
{
    net::io_context& ioc_;
    tcp::acceptor acceptor_;

public:
    listener(net::io_context& ioc, tcp::endpoint endpoint)
        : ioc_(ioc)
        , acceptor_(net::make_strand(ioc))
    {
        beast::error_code ec;

        acceptor_.open(endpoint.protocol(), ec);
        if (ec)
        {
            fail(ec, "open");
            return;
        }

        acceptor_.set_option(net::socket_base::reuse_address(true), ec);
        if (ec)
        {
            fail(ec, "set_option");
            return;
        }

        acceptor_.bind(endpoint, ec);
        if (ec)
        {
            fail(ec, "bind");
            return;
        }

        acceptor_.listen(net::socket_base::max_listen_connections, ec);
        if (ec)
        {
            fail(ec, "listen");
            return;
        }
    }

    void run()
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(net::make_strand(ioc_),
                               [self = shared_from_this()](beast::error_code ec, tcp::socket socket)
                               {
                                   if (!ec)
                                   {
                                       std::make_shared<session>(std::move(socket))->run();
                                   }
                                   else
                                   {
                                       fail(ec, "accept");
                                   }
                                   self->do_accept();
                               });
    }
};