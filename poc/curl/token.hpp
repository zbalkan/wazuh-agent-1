#pragma once

#include <boost/beast/core.hpp>
#include <string>
#include <unordered_map>
#include <chrono>
#include <ctime>

const std::string uuidKey = "uuid=";
const std::string tokenKey = "token=";
const std::string eventKey = "event=";
const std::string passwordKey = "password=";

void fail(boost::beast::error_code ec, char const* what) {
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

bool verifyPassword(const std::string& uuid, const std::string& password) {
    return true;
}
