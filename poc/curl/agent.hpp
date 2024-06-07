#pragma once

#include "events.hpp"
#include "requests.hpp"
#include "db/sqlite_wrapper.hpp"
#include "db/rocksdb_wrapper.hpp"

#include <memory>
#include <string>
#include <thread>


struct Agent
{
    Agent(const std::string& url, const std::string& uuid, const std::string& password, std::string& token)
    {
        // Login to get new token
        SendLoginRequest(url, uuid, password, token);

        // Start commands thread
        tCommands = std::make_unique<std::thread>(
            [&url, &uuid, &password, &token] ()
            {
                SendCommandsRequest(url, uuid, password, token);
            }
        );

        // Start events db
        eventsDb = std::make_unique<EventsDb<SQLiteWrapper>>(
            [&url, &uuid, &token] (const std::string& event)
            {
                return SendStatelessRequest(url, uuid, token, event);
            }
        );
    }

    ~Agent()
    {
        // Stop commands thread
        StopCommands();
        tCommands->join();
    }

    std::unique_ptr<std::thread> tCommands;
    std::unique_ptr<EventsDb<SQLiteWrapper>> eventsDb;
};