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

#include "db/dummy_wrapper.hpp"
#include "db/sqlite_wrapper.hpp"

#include "logger.hpp"

class EventQueueMonitorFixture : public benchmark::Fixture
{
public:
    EventQueueMonitorFixture()
    {
        Logger::LOGGING_ENABLED = false;
    }

    std::unique_ptr<EventQueueMonitor<DummyWrapper>> monitor;
};

BENCHMARK_DEFINE_F(EventQueueMonitorFixture, DispatchPreLoadedPendingEventsWithDifferentBatchSizes)(benchmark::State& state)
{
    std::atomic<int> counter = 0;

    for (auto _ : state)
    {
        monitor = std::make_unique<EventQueueMonitor<DummyWrapper>>([](const std::string&) { return false; });

        // First we stop the event loop so we can control that it starts with a preloaded db
        monitor->continueEventProcessing = false;

        // When all events are processed we stop the event loop
        monitor->shouldStopRunningIfQueueIsEmpty = true;

        // Preload db with events
        for (int i = 0; i < 100000; ++i)
        {
            monitor->eventQueue->InsertEvent(i, "event_data", "event_type");
        }

        // Benchmark the time it takes to dispatch all the events
        auto start = std::chrono::high_resolution_clock::now();
        monitor->batchSize = state.range(0);
        monitor->continueEventProcessing = true;
        monitor->Run(
            [&counter](const std::string& event_data) -> bool
            {
                // Avoid optimizing out the counter
                // maybe this is not necessary
                benchmark::DoNotOptimize(counter);
                counter++;
                benchmark::ClobberMemory();
                // std::this_thread::sleep_for(std::chrono::milliseconds(1));
                return true;
            });

        // We include the reset as part of the iteration time so we wait
        // for all the threads to finish
        monitor.reset();
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK_REGISTER_F(EventQueueMonitorFixture, DispatchPreLoadedPendingEventsWithDifferentBatchSizes)
    ->Unit(benchmark::kMillisecond)
    ->Iterations(100)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000);

BENCHMARK_MAIN();
