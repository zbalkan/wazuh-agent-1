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


#include "logger.hpp"
#include "client.hpp"

class ClientFixture : public benchmark::Fixture
{
public:
    ClientFixture()
    {
        Logger::LOGGING_ENABLED = false;
    }

    std::unique_ptr<Client> client;
};

BENCHMARK_DEFINE_F(ClientFixture, DispatchAndSend1000PreLoadedPendingEventsWithBatchSizesOf)
(benchmark::State& state)
{
    for (auto _ : state)
    {
        client = std::make_unique<Client>();

        // First we stop the event loop so we can control that it starts with a preloaded db
        client->eventQueueMonitor->continueEventProcessing = false;

        // Stop command dispatcher loop
        // client->commandDispatcher->keepCommandDispatcherRunning = false;

        // When all events are processed we stop the event loop
        client->eventQueueMonitor->shouldStopRunningIfQueueIsEmpty = true;

        // Preload db with events
        for (int i = 0; i < 1000; ++i)
        {
            client->eventQueueMonitor->eventQueue->InsertEvent(i, "event_data", "event_type");
        }

        // Benchmark the time it takes to dispatch all the events
        auto start = std::chrono::high_resolution_clock::now();
        client->eventQueueMonitor->batchSize = state.range(0);
        client->eventQueueMonitor->continueEventProcessing = true;
        client->eventQueueMonitor->Run(
            [this](const std::string& event)
            { return SendStatelessRequest(client->url, client->uuid, client->password, client->token, event); });

        // We include the reset as part of the iteration time so we wait
        // for all the threads to finish
        client->eventQueueMonitor->threadManager.JoinAll();
        auto end = std::chrono::high_resolution_clock::now();
        client->eventQueueMonitor.reset();
        // client->commandDispatcher.reset();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK_REGISTER_F(ClientFixture, DispatchAndSend1000PreLoadedPendingEventsWithBatchSizesOf)
    ->UseManualTime()
    ->Unit(benchmark::kMillisecond)
    ->Iterations(100)
    ->Arg(1)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1'000);

BENCHMARK_DEFINE_F(ClientFixture, DispatchPreLoadedPendingEventsWithFixedBatchSizeOf100AndEventCountOf)
(benchmark::State& state)
{
    for (auto _ : state)
    {
        client = std::make_unique<Client>();

        // First we stop the event loop so we can control that it starts with a preloaded db
        client->eventQueueMonitor->continueEventProcessing = false;

        // Stop command dispatcher loop
        // client->commandDispatcher->keepCommandDispatcherRunning = false;

        // When all events are processed we stop the event loop
        client->eventQueueMonitor->shouldStopRunningIfQueueIsEmpty = true;

        // Preload db with events
        for (int i = 0; i < state.range(0); ++i)
        {
            client->eventQueueMonitor->eventQueue->InsertEvent(i, "event_data", "event_type");
        }

        // Benchmark the time it takes to dispatch all the events
        auto start = std::chrono::high_resolution_clock::now();
        client->eventQueueMonitor->batchSize = 100;
        client->eventQueueMonitor->continueEventProcessing = true;
        client->eventQueueMonitor->Run(
            [this](const std::string& event)
            { return SendStatelessRequest(client->url, client->uuid, client->password, client->token, event); });

        // We include the reset as part of the iteration time so we wait
        // for all the threads to finish
        client->eventQueueMonitor->threadManager.JoinAll();
        auto end = std::chrono::high_resolution_clock::now();
        client->eventQueueMonitor.reset();
        // client->commandDispatcher.reset();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK_REGISTER_F(ClientFixture, DispatchPreLoadedPendingEventsWithFixedBatchSizeOf100AndEventCountOf)
    ->UseManualTime()
    ->Unit(benchmark::kMillisecond)
    ->Iterations(100)
    ->Arg(1)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1'000);

BENCHMARK_DEFINE_F(ClientFixture, Dispatch1000PreLoadedPendingEventsWithFixedBatchSizeOf100AndEventDataSizeOf)(benchmark::State& state)
{
    // Function to generate a string of N bytes
    auto generateString = [](size_t length, char fill_char = 'A')
    {
        return std::string(length, fill_char);
    };

    for (auto _ : state)
    {
        client = std::make_unique<Client>();

        // First we stop the event loop so we can control that it starts with a preloaded db
        client->eventQueueMonitor->continueEventProcessing = false;

        // Stop command dispatcher loop
        // client->commandDispatcher->keepCommandDispatcherRunning = false;

        // When all events are processed we stop the event loop
        client->eventQueueMonitor->shouldStopRunningIfQueueIsEmpty = true;

        // Preload db with events
        for (int i = 0; i < 1000; ++i)
        {
            client->eventQueueMonitor->eventQueue->InsertEvent(i, generateString(state.range(0)), "event_type");
        }

        // Benchmark the time it takes to dispatch all the events
        auto start = std::chrono::high_resolution_clock::now();
        client->eventQueueMonitor->batchSize = 100;
        client->eventQueueMonitor->continueEventProcessing = true;
        client->eventQueueMonitor->Run(
            [](const std::string& event_data) -> bool
            {
                return true;
            });

        // We include the reset as part of the iteration time so we wait
        // for all the threads to finish
        client->eventQueueMonitor->threadManager.JoinAll();
        auto end = std::chrono::high_resolution_clock::now();
        client->eventQueueMonitor.reset();

        auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
        state.SetIterationTime(elapsed_seconds.count());
    }
}

BENCHMARK_REGISTER_F(ClientFixture, Dispatch1000PreLoadedPendingEventsWithFixedBatchSizeOf100AndEventDataSizeOf)
    ->UseManualTime()
    ->Unit(benchmark::kMillisecond)
    ->Iterations(100)
    ->Arg(1)
    ->Arg(10)
    ->Arg(100)
    ->Arg(1'000)
    ->Arg(10'000)
    ->Arg(100'000);

BENCHMARK_MAIN();
