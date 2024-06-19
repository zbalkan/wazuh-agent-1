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

BENCHMARK_DEFINE_F(EventQueueMonitorFixture, Dispatch200000PreLoadedPendingEventsWithBatchSizesOf)
(benchmark::State& state)
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
        for (int i = 0; i < 200000; ++i)
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
        monitor->threadManager.JoinAll();
        auto end = std::chrono::high_resolution_clock::now();
        monitor.reset();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK_REGISTER_F(EventQueueMonitorFixture, Dispatch200000PreLoadedPendingEventsWithBatchSizesOf)
    ->UseManualTime()
    ->Unit(benchmark::kMillisecond)
    ->Iterations(100)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000)
    ->Arg(200'000);

BENCHMARK_DEFINE_F(EventQueueMonitorFixture, DispatchPreLoadedPendingEventsWithFixedBatchSizeOf100AndEventCountOf)
(benchmark::State& state)
{
    for (auto _ : state)
    {
        monitor = std::make_unique<EventQueueMonitor<DummyWrapper>>([](const std::string&) { return false; });

        // First we stop the event loop so we can control that it starts with a preloaded db
        monitor->continueEventProcessing = false;

        // When all events are processed we stop the event loop
        monitor->shouldStopRunningIfQueueIsEmpty = true;

        // Preload db with events
        for (int i = 0; i < state.range(0); ++i)
        {
            monitor->eventQueue->InsertEvent(i, "event_data", "event_type");
        }

        // Benchmark the time it takes to dispatch all the events
        auto start = std::chrono::high_resolution_clock::now();
        monitor->batchSize = 100;
        monitor->continueEventProcessing = true;
        monitor->Run([](const std::string& event_data) -> bool { return true; });

        // We include the reset as part of the iteration time so we wait
        // for all the threads to finish
        monitor->threadManager.JoinAll();
        auto end = std::chrono::high_resolution_clock::now();
        monitor.reset();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK_REGISTER_F(EventQueueMonitorFixture, DispatchPreLoadedPendingEventsWithFixedBatchSizeOf100AndEventCountOf)
    ->UseManualTime()
    ->Unit(benchmark::kMillisecond)
    ->Iterations(100)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000)
    ->Arg(200'000)
    ->Arg(300'000)
    ->Arg(500'000)
    ->Arg(1'000'000);

BENCHMARK_DEFINE_F(EventQueueMonitorFixture, Dispatch100000PreLoadedPendingEventsWithFixedBatchSizeOf100AndEventDataSizeOf)(benchmark::State& state)
{
    // Function to generate a string of N bytes
    auto generateString = [](size_t length, char fill_char = 'A')
    {
        return std::string(length, fill_char);
    };

    for (auto _ : state)
    {
        monitor = std::make_unique<EventQueueMonitor<DummyWrapper>>([](const std::string&) { return false; });

        // First we stop the event loop so we can control that it starts with a preloaded db
        monitor->continueEventProcessing = false;

        // When all events are processed we stop the event loop
        monitor->shouldStopRunningIfQueueIsEmpty = true;

        // Preload db with events
        for (int i = 0; i < 100'000; ++i)
        {
            monitor->eventQueue->InsertEvent(i, generateString(state.range(0)), "event_type");
        }

        // Benchmark the time it takes to dispatch all the events
        auto start = std::chrono::high_resolution_clock::now();
        monitor->batchSize = 100;
        monitor->continueEventProcessing = true;
        monitor->Run(
            [](const std::string& event_data) -> bool
            {
                return true;
            });

        // We include the reset as part of the iteration time so we wait
        // for all the threads to finish
        monitor->threadManager.JoinAll();
        auto end = std::chrono::high_resolution_clock::now();
        monitor.reset();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK_REGISTER_F(EventQueueMonitorFixture, Dispatch100000PreLoadedPendingEventsWithFixedBatchSizeOf100AndEventDataSizeOf)
    ->UseManualTime()
    ->Unit(benchmark::kMillisecond)
    ->Iterations(100)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000)
    ->Arg(250'000)
    ->Arg(500'000)
    ->Arg(1'000'000);

BENCHMARK_MAIN();
