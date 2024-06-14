#pragma once

#include <boost/beast/core.hpp>

#include <string>

const std::string uuidKey = "\"uuid\":";
const std::string tokenKey = "token=";
const std::string eventKey = "\"event\":";
const std::string passwordKey = "\"password\":";
const std::string nameKey = "name=";
const std::string ipKey = "ip=";
const std::string bearerPrefix = "Bearer ";

const std::string kURL = "http://localhost:5000";
const std::string kUUID = "018fe477-31c8-7580-ae4a-e0b36713eb05";
const std::string kPASSWORD = "123456";
const std::string kNAME = "agent_name";
const std::string kIP = "agent_ip";

void fail(boost::beast::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}
