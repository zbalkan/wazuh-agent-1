#pragma once

#include "event_queue_monitor.hpp"
#include "command_dispatcher.hpp"
#include "requests.hpp"
#include "db/sqlite_wrapper.hpp"
#include "db/rocksdb_wrapper.hpp"

#include <memory>
#include <string>
#include <thread>


struct Client
{
    Client(const std::string& url, const std::string& uuid, const std::string& password, std::string& token)
    {
        // Login to get new token
        SendLoginRequest(url, uuid, password, token);

        // Start command dispatcher
        commandDispatcher = std::make_unique<CommandDispatcher<RocksDBWrapper>>(url, uuid, password, token);

        // Start queue monitoring
        eventQueueMonitor = std::make_unique<EventQueueMonitor<SQLiteWrapper>>(
            [&url, &uuid, &token] (const std::string& event)
            {
                return SendStatelessRequest(url, uuid, token, event);
            }
        );
    }

    std::unique_ptr<EventQueueMonitor<SQLiteWrapper>> eventQueueMonitor;
    std::unique_ptr<CommandDispatcher<RocksDBWrapper>> commandDispatcher;
};