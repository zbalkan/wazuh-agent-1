#include "event_queue_monitor.hpp" // Adjust the include path as necessary
#include <atomic>
#include <benchmark/benchmark.h>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include "db/sqlite_wrapper.hpp"
#include "logger.hpp"


static void BM_EventQueueMonitor_DispatchPendingEvents(benchmark::State& state)
{
    Logger::LOGGING_ENABLED = false;

    auto onEvent = [](const std::string& event_data) -> bool
    {
        return true;
    };

    for (auto _ : state)
    {
        EventQueueMonitor<SQLiteWrapper> monitor(onEvent);

        std::vector<Event> pending_events(state.range(0));
        for (int i = 0; i < state.range(0); ++i)
        {
            monitor.eventQueue->InsertEvent(i, "event_data", "event_type");
        }
    }
}

BENCHMARK(BM_EventQueueMonitor_DispatchPendingEvents)->Arg(10)->Arg(100)->Arg(1000);

BENCHMARK_MAIN();
