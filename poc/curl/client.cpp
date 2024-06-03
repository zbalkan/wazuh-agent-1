#include <iostream>
#include <string>
#include "HTTPRequest.hpp"
#include "IURLRequest.hpp"

#include "token.hpp"

std::string session_token {};

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

void SendLoginRequest(const std::string& pUrl, const std::string& uuid, const std::string& password) {
    try {
        HttpURL url {pUrl + "/login"};
        HTTPRequest& httpRequest = HTTPRequest::instance();
        std::string data = uuidKey + uuid + "&" + passwordKey + password;
        httpRequest.post(url, data,
                         [](const std::string& response) {
                            std::cout << "Login Response: " << response << std::endl;
                            session_token = response;
                         },
                         [](const std::string& error, const long code) {
                             std::cerr << "Login Request failed: " << error << " with code " << code << std::endl;
                         });
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void SendStatelessRequest(const std::string& pUrl, const std::string& uuid, const std::string& token, const std::string& event) {
    try {
        std::string authHeader = "Authorization: " + bearerPrefix + session_token;
        auto HeadersWithToken = DEFAULT_HEADERS;
        HeadersWithToken.insert(authHeader);

        HttpURL url {pUrl + "/stateless"};
        HTTPRequest& httpRequest = HTTPRequest::instance();
        std::string data = uuidKey + uuid + "&" + eventKey + event;
        httpRequest.post(url, data,
                         [](const std::string& response) {
                             std::cout << "Stateless Response: " << response << std::endl;
                         },
                         [](const std::string& error, const long code) {
                             std::cerr << "Stateless Request failed: " << error << " with code " << code << std::endl;
                         },
                         "",
                         HeadersWithToken);
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void SendCommandsRequest(const std::string& pUrl, const std::string& token) {
    try {
        HttpURL url {pUrl + "/commands"};
        HTTPRequest& httpRequest = HTTPRequest::instance();
        std::string data = "token=" + token;
        httpRequest.post(url, data,
                         [](const std::string& response) {
                             std::cout << "Commands Response: " << response << std::endl;
                         },
                         [](const std::string& error, const long code) {
                             std::cerr << "Commands Request failed: " << error << " with code " << code << std::endl;
                         });
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

int main() {
    std::string url = "http://localhost:8080";
    std::string uuid = "agent_uuid";
    std::string password = "123456";

    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }
        else if (command == "login") {
            SendLoginRequest(url, uuid, password);
        }
        else if (command == "stateless") {
            SendStatelessRequest(url, uuid, session_token, command);
        }
        else if (command == "commands") {
            SendCommandsRequest(url, session_token);
        }
        else if (command == "get") {
            SendGetRequest(url);
        }
        else if (command == "post") {
            std::string postData = "Hello, this is a POST request.";
            SendPostRequest(url, postData);
        }
        else if (command == "cleartoken") {
            session_token.clear();
        }
        else {
            std::cout << "Available commands: login, stateless, commands, get, post, cleartoken, exit\n" << std::endl;
        }
    }

    return 0;
}
