#pragma once

#include "requests.hpp"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <functional>
#include <atomic>
#include <memory>


template <typename CommandDB>
struct CommandDispatcher
{
    CommandDispatcher(const std::string& url, const std::string& uuid, const std::string& password, std::string& token)
    {
        commandDb = std::make_unique<CommandDB>("commandsRocksDb.db");
        commandDb->createTable();

        std::cout << "Starting command dispatcher thread\n";
        sender_thread = std::make_unique<std::thread>([this, &url, &uuid, &password, &token] () {sendCommandsRequests(url, uuid, password, token);});
        dispatcher_thread = std::make_unique<std::thread>([this] () {dispatcher();});
    }

    ~CommandDispatcher()
    {
        keepCommandDispatcherRunning.store(false);
        std::cout << "Waiting for commands threads to join\n";
        sender_thread->join();
        sender_thread.reset();
        dispatcher_thread->join();
        dispatcher_thread.reset();
        commandDb.reset();
    }

    void sendCommandsRequests (const std::string& url, const std::string& uuid, const std::string& password, std::string& token)
    {
        while (keepCommandDispatcherRunning.load()) {
            auto response = SendCommandsRequest(url, uuid, password, token);
            bool success = response.first;
            if (success) {
                commandDb->insertCommand(response.second);
            }
        }
    }

    void dispatcher()
    {
        while (keepCommandDispatcherRunning.load()) {
            const auto& pending_command = commandDb->fetchPendingCommand();
            if (pending_command.id != -1) {
                std::cout << "Dispatching command ID: " << pending_command.id << ", Data: " << pending_command.command_data << std::endl;
                commandDb->updateCommandStatus(pending_command.id);
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    std::atomic<bool> keepCommandDispatcherRunning = true;
    std::unique_ptr<std::thread> dispatcher_thread;
    std::unique_ptr<std::thread> sender_thread;
    std::unique_ptr<CommandDB> commandDb;
};
