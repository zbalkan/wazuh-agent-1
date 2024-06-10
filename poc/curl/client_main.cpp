#include <iostream>
#include <string>

#include "client.hpp"
#include "logger.hpp"
#include "defs.hpp"
#include "requests.hpp"
#include "token.hpp"
#include "event_queue_monitor.hpp"

std::string session_token {};


int main()
{
    Client client(kURL, kUUID, kPASSWORD, session_token);

    std::string command;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            break;
        }
        else if (command == "login") {
            SendLoginRequest(kURL, kUUID, kPASSWORD, session_token);
        }
        else if (command == "stateless") {
            SendStatelessRequest(kURL, kUUID, session_token, "");
        }
        else if (command == "stopcommands") {
            client.commandDispatcher->keepCommandDispatcherRunning.store(false);
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
            client.eventQueueMonitor->eventQueue->insertEvent(event++, "{\"key\": \"value\"}", "json");
            client.eventQueueMonitor->eventQueue->insertEvent(event++, "<event><key>value</key></event>", "xml");
        }
        else {
            std::cout << "Available commands: login, stateless, stopcommands, get, post, cleartoken, createevent, exit\n" << std::endl;
        }
    }

    Logger::log("MAIN", "Main thread is exiting.");
    return 0;
}
