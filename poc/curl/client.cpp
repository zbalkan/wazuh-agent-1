#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "HTTPRequest.hpp"
#include "IURLRequest.hpp"

#include "token.hpp"
#include "events.hpp"
#include "db/sqlite_wrapper.hpp"
#include "db/rocksdb_wrapper.hpp"

std::string session_token {};

const std::string kURL = "http://localhost:8080";
const std::string kUUID = "agent_uuid";
const std::string kPASSWORD = "123456";

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
            auto needLogin = false;

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

void suscribeToCommands(const std::string& url, const std::string& uuid, const std::string& password, std::string& token) {
    SendLoginRequest(url, uuid, password, token);
    SendCommandsRequest(url, uuid, password, token);
}

int main() {
    std::thread tCommands([&kURL, &kUUID, &kPASSWORD, &session_token]() {
        suscribeToCommands(kURL, kUUID, kPASSWORD, session_token);
    });

    EventsDb<SQLiteWrapper> eventsDb([&kURL, &kUUID, &session_token](const std::string& event) {
        SendStatelessRequest(kURL, kUUID, session_token, event);
    });

    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            keepRunning.store(false);
            condition.notify_one();
            break;
        }
        else if (command == "login") {
            SendLoginRequest(kURL, kUUID, kPASSWORD, session_token);
        }
        else if (command == "stateless") {
            SendStatelessRequest(kURL, kUUID, session_token, "");
        }
        else if (command == "get") {
            SendGetRequest(kURL);
        }
        else if (command == "post") {
            std::string postData = "Hello, this is a POST request.";
            SendPostRequest(kURL, postData);
        }
        else if (command == "cleartoken") {
            session_token.clear();
        }
        else if (command == "createevent") {
            static int event = 0;
            eventsDb.db->insertEvent(event++, "{\"key\": \"value\"}", "json");
            eventsDb.db->insertEvent(event++, "<event><key>value</key></event>", "xml");
        }
        else {
            std::cout << "Available commands: login, stateless, commands, get, post, cleartoken, createevent, exit\n" << std::endl;
        }
    }

    tCommands.join();
    std::cout << "Main thread is exiting.\n";
    return 0;
}
