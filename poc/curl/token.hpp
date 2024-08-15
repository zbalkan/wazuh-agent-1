#pragma once

#include <jwt-cpp/jwt.h>

#include <chrono>
#include <ctime>
#include <string>
#include <unordered_map>

struct TokenInfo
{
    std::string token;
    std::time_t expiry;
};

std::unordered_map<std::string, TokenInfo> validTokens;

std::string generateToken()
{
    std::string token = "new_token";
    return token;
}

bool verifyPassword(const std::string& user, const std::string& password)
{
    return true;
}

bool verifyUuid(const std::string& uuid)
{
    return true;
}

std::string createToken(const std::string& uuid)
{
    auto token = jwt::create()
                     .set_issuer("some-auth-server")
                     .set_type("JWS")
                     .set_payload_claim("uuid", jwt::claim(std::string(uuid)))
                     .set_issued_at(std::chrono::system_clock::now())
                     .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds {30})
                     .sign(jwt::algorithm::hs256 {"your-secret-key"});

    return token;
}

bool verifyToken(const std::string& token)
{
    try
    {
        auto decoded = jwt::decode(token);

        auto verifier =
            jwt::verify().allow_algorithm(jwt::algorithm::hs256 {"your-secret-key"}).with_issuer("some-auth-server");

        verifier.verify(decoded);

        if (decoded.has_payload_claim("uuid")) {
            auto uuid_claim = decoded.get_payload_claim("uuid");
            if (validTokens[uuid_claim.as_string()].token == token)
            {
                return true;
            }
        } else {
            throw std::runtime_error("UUID claim not found in the token");
        }

        return false;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Token verification failed: " << e.what() << std::endl;
        return false;
    }
}
