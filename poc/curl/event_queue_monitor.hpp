#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "logger.hpp"

template<typename QueueDB>
struct EventQueueMonitor
{
    EventQueueMonitor(std::function<bool(const std::string&)> onEvent)
    {
        eventQueue = std::make_unique<QueueDB>();
        eventQueue->createTable();
        eventQueue->updateEntriesStatus("processing", "pending");

        Logger::log("EVENT QUEUE MONITOR", "Starting event queue thread");
        dispatcher_thread = std::make_unique<std::thread>([this, onEvent]() { dispatcher(onEvent); });
    }

    ~EventQueueMonitor()
    {
        keepDbRunning.store(false);
        Logger::log("EVENT QUEUE MONITOR", "Waiting for event queue thread to join");
        for (auto& thread : eventDispatchThreads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
        dispatcher_thread->join();
        dispatcher_thread.reset();
        eventQueue.reset();
        Logger::log("EVENT QUEUE MONITOR", "Destroyed");
    }

    void dispatcher(std::function<bool(const std::string&)> onEvent)
    {
        // This should be part of the configuration
        const int N = 10; // Number of events to dispatch at once
        const int T = 5;  // Time interval in seconds

        auto last_dispatch_time = std::chrono::steady_clock::now();

        while (keepDbRunning.load())
        {
            CleanUpDispatchedEvents();
            CleanUpJoinableThreads();

            const auto current_time = std::chrono::steady_clock::now();

            if (eventQueue->getPendingEventCount() < N &&
                std::chrono::duration_cast<std::chrono::seconds>(current_time - last_dispatch_time).count() < T)
            {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            if (auto pending_events = eventQueue->fetchAndMarkPendingEvents(N); !pending_events.empty())
            {
                std::vector<int> event_ids;
                std::string event_data;

                // Aggregate event data for the batch to dispatch
                for (const auto& event : pending_events)
                {
                    Logger::log("EVENT QUEUE MONITOR",
                                "Dispatching event ID: " + std::to_string(event.id) + ", Data: " + event.event_data);
                    event_ids.push_back(event.id);
                    event_data += event.event_data;
                    event_data += "\n";
                }

                // Create a new thread for each event batch to dispatch
                eventDispatchThreads.emplace_back(
                    [this, onEvent, event_data, event_ids]()
                    {
                        if (onEvent(event_data))
                        {
                            eventQueue->updateEventStatus(event_ids, "dispatched");
                        }
                        else
                        {
                            eventQueue->updateEventStatus(event_ids, "pending");
                        }
                    });
            }
            last_dispatch_time = current_time;
        }
    }

    void CleanUpDispatchedEvents()
    {
        // Cleanup dispatched events
        // (this could be done in a separate thread as well, maybe by a different class)
        eventQueue->deleteEntriesWithStatus("dispatched");
    }

    void CleanUpJoinableThreads()
    {
        auto it = std::remove_if(eventDispatchThreads.begin(),
                                 eventDispatchThreads.end(),
                                 [](std::thread& t)
                                 {
                                     if (t.joinable())
                                     {
                                         t.join();
                                         return true;
                                     }
                                     return false;
                                 });

        eventDispatchThreads.erase(it, eventDispatchThreads.end());
    }

    std::atomic<bool> keepDbRunning = true;
    std::unique_ptr<std::thread> dispatcher_thread;
    std::unique_ptr<QueueDB> eventQueue;
    std::vector<std::thread> eventDispatchThreads;
};
