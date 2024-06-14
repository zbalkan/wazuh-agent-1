#pragma once

#include <atomic>
#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "logger.hpp"
#include "db_wrapper.hpp"

template<typename QueueDB>
struct EventQueueMonitor
{
    EventQueueMonitor(std::function<bool(const std::string&)> onEvent)
    {
        eventQueue = std::make_unique<QueueDB>();
        eventQueue->CreateTable();
        eventQueue->UpdateEntriesStatus("processing", "pending");

        Logger::log("EVENT QUEUE MONITOR", "Starting event queue thread");
        dispatcher_thread = std::make_unique<std::thread>([this, onEvent]() { Run(onEvent); });
    }

    ~EventQueueMonitor()
    {
        continueEventProcessing.store(false);
        Logger::log("EVENT QUEUE MONITOR", "Waiting for event queue thread to join");

        if (dispatcher_thread && dispatcher_thread->joinable())
        {
            dispatcher_thread->join();
        }

        for (auto& thread : eventDispatchThreads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }

        dispatcher_thread.reset();
        eventQueue.reset();
        Logger::log("EVENT QUEUE MONITOR", "Destroyed");
    }

    void Run(std::function<bool(const std::string&)> onEvent)
    {
        auto last_dispatch_time = std::chrono::steady_clock::now();

        while (continueEventProcessing.load())
        {
            PerformCleanup();

            if (!ShouldDispatchEvents(last_dispatch_time))
            {
                // std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            if (auto pending_events = eventQueue->FetchAndMarkPendingEvents(batchSize); !pending_events.empty())
            {
                DispatchPendingEvents(onEvent, pending_events);
            }

            last_dispatch_time = std::chrono::steady_clock::now();
        }
    }

    void PerformCleanup()
    {
        CleanUpDispatchedEvents();
        CleanUpJoinableThreads();
    }

    bool ShouldDispatchEvents(const std::chrono::time_point<std::chrono::steady_clock>& last_dispatch_time)
    {
        const auto current_time = std::chrono::steady_clock::now();
        return eventQueue->GetPendingEventCount() >= batchSize ||
               std::chrono::duration_cast<std::chrono::seconds>(current_time - last_dispatch_time).count() >=
                   dispatchInterval;
    }

    void DispatchPendingEvents(const std::function<bool(const std::string&)>& onEvent,
                               const std::vector<Event>& pending_events)
    {
        std::vector<int> event_ids;
        std::string event_data = "[";

        for (size_t i = 0; i < pending_events.size(); ++i)
        {
            const auto& event = pending_events[i];

            Logger::log("EVENT QUEUE MONITOR",
                        "Dispatching event ID: " + std::to_string(event.id) + ", Data: " + event.event_data);

            event_ids.push_back(event.id);
            event_data += event.event_data;

            if (i != pending_events.size() - 1)
            {
                event_data += ",";
            }
        }

        event_data += "]";

        // Create a new thread for each event batch to dispatch
        eventDispatchThreads.emplace_back([this, onEvent, event_data, event_ids]()
                                          { UpdateEventStatus(onEvent(event_data), event_ids); });
        ++eventCount;

        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = currentTime - startTime;

        if (elapsed.count() > 0) {
            std::cout << "Request per second: " << eventCount / elapsed.count() << std::endl;
            // if (elapsed.count() >= 1) {
            //     eventCount = 0;
            //     startTime = std::chrono::high_resolution_clock::now();
            // }
        }
    }

    void UpdateEventStatus(bool success, const std::vector<int>& event_ids)
    {
        if (success)
        {
            eventQueue->UpdateEventStatus(event_ids, "dispatched");
        }
        else
        {
            eventQueue->UpdateEventStatus(event_ids, "pending");
        }
    }

    void CleanUpDispatchedEvents()
    {
        // Cleanup dispatched events
        // (this could be done in a separate thread as well, maybe by a different class)
        eventQueue->DeleteEntriesWithStatus("dispatched");
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

    std::atomic<bool> continueEventProcessing = true;
    std::unique_ptr<std::thread> dispatcher_thread;
    std::unique_ptr<QueueDB> eventQueue;
    std::vector<std::thread> eventDispatchThreads;

    // Configuration constants
    const int batchSize = 1;       // Number of events to dispatch at once
    const int dispatchInterval = 0; // Time interval in seconds
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
    int eventCount = 0;
};
