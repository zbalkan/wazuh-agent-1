#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <functional>
#include <atomic>

std::atomic<bool> keepDbRunning(true);

template <typename DB>
void dispatcher(DB* dbWrapper, std::function<bool(const std::string&)> onEvent) {
    const int N = 10;  // Number of events to dispatch at once
    const int T = 5;   // Time interval in seconds

    auto last_dispatch_time = std::chrono::steady_clock::now();

    while (keepDbRunning.load()) {
        const auto current_time = std::chrono::steady_clock::now();

        if (dbWrapper->getPendingEventCount() < N && std::chrono::duration_cast<std::chrono::seconds>(current_time - last_dispatch_time).count() < T) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        auto pending_events = dbWrapper->fetchAndMarkPendingEvents(N);
        if (!pending_events.empty()) {
            // Dispatch logic here
            std::vector<int> event_ids;
            std::string event_data;
            for (const auto& event : pending_events) {
                std::cout << "Dispatching event ID: " << event.id << ", Data: " << event.event_data << std::endl;
                event_ids.push_back(event.id);
                event_data += event.event_data;
                event_data += "\n";
            }

            if (onEvent(event_data))
            {
                dbWrapper->updateEventStatus(event_ids, "dispatched");
            }
            else
            {
                dbWrapper->updateEventStatus(event_ids, "pending");
            }
        }
        last_dispatch_time = current_time;
    }
}

template <typename QueueDB>
struct EventQueueMonitor
{
    EventQueueMonitor(std::function<bool(const std::string&)> pOnEvent)
    : onEvent(pOnEvent)
    {
        eventQueue = std::make_unique<QueueDB>();
        eventQueue->createTable();
        std::cout << "Starting event eventQueue thread\n";
        dispatcher_thread = std::make_unique<std::thread>(dispatcher<QueueDB>, eventQueue.get(), onEvent);
    }

    ~EventQueueMonitor()
    {
        keepDbRunning.store(false);
        std::cout << "Waiting for event eventQueue thread to join\n";
        dispatcher_thread->join();
        dispatcher_thread.reset();
        eventQueue.reset();
    }

    std::unique_ptr<std::thread> dispatcher_thread;
    std::unique_ptr<QueueDB> eventQueue;
    std::function<bool(const std::string&)> onEvent;
};
