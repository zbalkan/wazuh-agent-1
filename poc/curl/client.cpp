#include <iostream>
#include <string>
#include "HTTPRequest.hpp"

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
                             session_token = response;
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
        std::string data = "uuid=" + uuid + "&password=" + password;
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

void SendStatelessRequest(const std::string& pUrl, const std::string& token, const std::string& event) {
    try {
        HttpURL url {pUrl + "/stateless"};
        HTTPRequest& httpRequest = HTTPRequest::instance();
        std::string data = "token=" + token + "&event=" + event;
        httpRequest.post(url, data,
                         [](const std::string& response) {
                             std::cout << "Stateless Response: " << response << std::endl;
                         },
                         [](const std::string& error, const long code) {
                             std::cerr << "Stateless Request failed: " << error << " with code " << code << std::endl;
                         });
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

    // Send a GET request
    SendGetRequest(url);

    // Send a POST request
    std::string postData = "Hello, this is a POST request.";
    SendPostRequest(url, postData);

    // Send a login request
    std::string uuid = "agent_uuid";
    std::string password = "123456";
    SendLoginRequest(url, uuid, password);

    SendStatelessRequest(url, session_token, "event");
    SendCommandsRequest(url, session_token);

    // todo review content type of http request (could be text/plain or application/json, or xml)
    // ie Content-Type: text/xml; charset=utf-8
    // check DEFAULT_HEADERS in IURLRequest.hpp

    return 0;
}
