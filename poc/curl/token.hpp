#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <string>
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
