#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <functional>

std::atomic<bool> keepDbRunning(true);

template <typename DB>
void dispatcher(DB* dbWrapper, std::function<void(const std::string&)> onEvent) {
    const int N = 10;  // Number of events to dispatch at once
    const int T = 5;   // Time interval in seconds

    while (keepDbRunning.load()) {
        auto pending_events = dbWrapper->fetchPendingEvents(N);
        if (!pending_events.empty()) {
            // Dispatch logic here
            std::vector<int> event_ids;
            std::string event_data;
            for (const auto& event : pending_events) {
                std::cout << "Dispatching event ID: " << event.id << ", Data: " << event.event_data << std::endl;
                event_ids.push_back(event.id);
                event_data += event.event_data;
            }
            onEvent(event_data);
            dbWrapper->updateEventStatus(event_ids);
        }
        std::this_thread::sleep_for(std::chrono::seconds(T));
    }
}

template <typename DB>
struct EventsDb
{
    EventsDb(std::function<void(const std::string&)> pOnEvent)
    : onEvent(pOnEvent)
    {
        db = std::make_unique<DB>();
        db->createTable();
        std::cout << "Starting event db thread\n";
        dispatcher_thread = std::make_unique<std::thread>(dispatcher<DB>, db.get(), onEvent);
    }

    ~EventsDb()
    {
        keepDbRunning.store(false);
        std::cout << "Waiting for event db thread to join\n";
        dispatcher_thread->join();
        dispatcher_thread.reset();
        db.reset();
    }

    std::unique_ptr<std::thread> dispatcher_thread;
    std::unique_ptr<DB> db;
    std::function<void(const std::string&)> onEvent;
};
