#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "HTTPRequest.hpp"
#include "IURLRequest.hpp"

#include "defs.hpp"
#include "token.hpp"


std::condition_variable condition;
std::atomic<bool> keepRunning(true);


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

void SendLoginRequest(const std::string& pUrl, const std::string& uuid, const std::string& password, std::string& token) {
    try {
        HttpURL url {pUrl + "/login"};
        HTTPRequest& httpRequest = HTTPRequest::instance();
        std::string data = uuidKey + uuid + "&" + passwordKey + password;
        httpRequest.post(url, data,
                         [&token](const std::string& response) {
                            std::cout << "Login Response: " << response << std::endl;
                            token = response;
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
        std::string authHeader = "Authorization: " + bearerPrefix + token;
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

void SendCommandsRequest(const std::string& pUrl, const std::string& uuid, const std::string& password, std::string& token) {
    while (keepRunning.load()) {
        try {
            std::string authHeader = "Authorization: " + bearerPrefix + token;
            auto HeadersWithToken = DEFAULT_HEADERS;
            HeadersWithToken.insert(authHeader);

            HttpURL url {pUrl + "/commands" + "?" + uuidKey + uuid };
            HTTPRequest& httpRequest = HTTPRequest::instance();
            httpRequest.get(url,
                            [](const std::string& response) {
                                std::cout << "Commands Response: " << response << std::endl;
                            },
                            [pUrl, uuid, password, &token](const std::string& error, const long code) {
                                std::cerr << "Commands Request failed: " << error << " with code " << code << std::endl;
                                if (code == 401) {
                                    SendLoginRequest(pUrl, uuid, password, token);
                                }
                            },
                            "",
                            HeadersWithToken);
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }
}

void subscribeToCommands(const std::string& url, const std::string& uuid, const std::string& password, std::string& token) {
    SendLoginRequest(url, uuid, password, token);
    SendCommandsRequest(url, uuid, password, token);
}

void StopCommands() {
    keepRunning.store(false);
    condition.notify_one();
}
