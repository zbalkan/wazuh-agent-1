#include <iostream>
#include <string>
#include "HTTPRequest.hpp"

void SendGetRequest(const std::string& pUrl) {
    try {
        HttpURL url {pUrl};
        HTTPRequest& httpRequest = HTTPRequest::instance();
        httpRequest.get(url,
                        [](const std::string& response) {
                            std::cout << "GET Response: " << response << std::endl;
                        },
                        [](const std::string& error, const long code) {
                            std::cerr << "GET Request failed: " << error << " with code " << code << std::endl;
                        });
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void SendPostRequest(const std::string& pUrl, const std::string& data) {
    try {
        HttpURL url {pUrl};
        HTTPRequest& httpRequest = HTTPRequest::instance();
        httpRequest.post(url, data,
                         [](const std::string& response) {
                             std::cout << "POST Response: " << response << std::endl;
                         },
                         [](const std::string& error, const long code) {
                             std::cerr << "POST Request failed: " << error << " with code " << code << std::endl;
                         });
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void SendLoginRequest(const std::string& pUrl, const std::string& uuid, const std::string& token) {
    try {
        HttpURL url {pUrl + "/login"};
        HTTPRequest& httpRequest = HTTPRequest::instance();
        std::string data = "uuid=" + uuid + "&token=" + token;
        httpRequest.post(url, data,
                         [](const std::string& response) {
                             std::cout << "Login Response: " << response << std::endl;
                         },
                         [](const std::string& error, const long code) {
                             std::cerr << "Login Request failed: " << error << " with code " << code << std::endl;
                         });
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    std::string url = "http://localhost:8080";

    // Send a GET request
    SendGetRequest(url);

    // Send a POST request
    std::string postData = "Hello, this is a POST request.";
    SendPostRequest(url, postData);

    // Send a login request
    std::string uuid = "agent_uuid";
    std::string token = "previously_generated_token";
    SendLoginRequest(url, uuid, token);

    return 0;
}
