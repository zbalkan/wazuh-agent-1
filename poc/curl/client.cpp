#include <iostream>
#include <string>
#include <thread>

#include "defs.hpp"
#include "requests.hpp"
#include "token.hpp"
#include "events.hpp"
#include "db/sqlite_wrapper.hpp"
#include "db/rocksdb_wrapper.hpp"

std::string session_token {};


int main() {
    std::thread tCommands([&kURL, &kUUID, &kPASSWORD, &session_token]() {
        subscribeToCommands(kURL, kUUID, kPASSWORD, session_token);
    });

    EventsDb<SQLiteWrapper> eventsDb([&kURL, &kUUID, &session_token](const std::string& event) {
        SendStatelessRequest(kURL, kUUID, session_token, event);
    });

    std::string command;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, command);

        if (command == "exit") {
            StopCommands();
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
