#pragma once

#include "command_dispatcher.hpp"
#include "db/rocksdb_wrapper.hpp"
#include "db/sqlite_wrapper.hpp"
#include "event_queue_monitor.hpp"
#include "logger.hpp"
#include "requests.hpp"

#include <memory>
#include <string>
#include <thread>

struct Client
{
    Client()
    {
        Logger::log("CLIENT", "Starting client...");

        ReadCredentials();

        // Login to get new token
        SendLoginRequest(url, uuid, password, token);

        // Start command dispatcher
        commandDispatcher = std::make_unique<CommandDispatcher<RocksDBWrapper>>(url, uuid, password, token);

        // Start queue monitoring
        eventQueueMonitor = std::make_unique<EventQueueMonitor<SQLiteWrapper>>(
            [this](const std::string& event)
            { return SendStatelessRequest(this->url, this->uuid, this->token, event); });
    }

    void ReadCredentials()
    {
        Logger::log("CLIENT", "Reading credentials...");
        url = kURL;
        uuid = kUUID;
        password = kPASSWORD;
    }

    std::string url;
    std::string uuid;
    std::string password;
    std::string token;
    std::unique_ptr<EventQueueMonitor<SQLiteWrapper>> eventQueueMonitor;
    std::unique_ptr<CommandDispatcher<RocksDBWrapper>> commandDispatcher;
};