#pragma once

#include "logger.hpp"

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>


template<typename CommandDB>
struct CommandDispatcher
{
    CommandDispatcher(std::function<std::pair<bool, std::string>()> onCommand)
    {
        Logger::log("COMMAND DISPATCHER", "Starting command dispatcher thread");

        commandDb = std::make_unique<CommandDB>("commandsRocksDb.db");
        commandDb->CreateTable();

        sender_thread = std::make_unique<std::thread>([this, onCommand]()
                                                      { SendCommandsRequests(onCommand); });
        dispatcher_thread = std::make_unique<std::thread>([this]() { Run(); });
    }

    ~CommandDispatcher()
    {
        keepCommandDispatcherRunning.store(false);
        Logger::log("COMMAND DISPATCHER", "Waiting for commands threads to join");
        sender_thread->join();
        sender_thread.reset();
        dispatcher_thread->join();
        dispatcher_thread.reset();
        commandDb.reset();
        Logger::log("COMMAND DISPATCHER", "Destroyed");
    }

    void SendCommandsRequests(std::function<std::pair<bool, std::string>()> onCommand)
    {
        while (keepCommandDispatcherRunning.load())
        {
            if (const auto response = onCommand(); response.first)
            {
                auto jsonResponse = nlohmann::json::parse(response.second);
                if (jsonResponse.contains("commands") && jsonResponse["commands"].is_array()) {
                    for (const auto& command : jsonResponse["commands"]) {
                        if (command.is_object()) {
                            commandDb->InsertCommand(command.dump());
                        }
                    }
                } else {
                    std::cerr << "Error: 'commands' key not found or is not an array in the response" << std::endl;
                }
            }
        }
    }

    void Run()
    {
        while (keepCommandDispatcherRunning.load())
        {
            const auto& pending_command = commandDb->FetchPendingCommand();
            if (pending_command.id != -1)
            {
                Logger::log("COMMAND DISPATCHER",
                            "Dispatching command ID: " + std::to_string(pending_command.id) +
                                ", Data: " + pending_command.command_data);
                commandDb->UpdateCommandStatus(pending_command.id);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void StartCommandDispatcher()
    {
        dispatcher_thread->join();
        dispatcher_thread.reset();
        keepCommandDispatcherRunning.store(true);
        dispatcher_thread = std::make_unique<std::thread>([this]() { Run(); });
    }

    std::atomic<bool> keepCommandDispatcherRunning = false;
    std::unique_ptr<std::thread> dispatcher_thread;
    std::unique_ptr<std::thread> sender_thread;
    std::unique_ptr<CommandDB> commandDb;
};
