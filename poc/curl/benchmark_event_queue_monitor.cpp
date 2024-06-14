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

class EventQueueMonitorFixture : public benchmark::Fixture
{
public:
    void SetUp(const ::benchmark::State& state) override
    {
        Logger::LOGGING_ENABLED = false;

        onEvent = [](const std::string& event_data) -> bool
        {
            return true;
        };

        monitor = std::make_unique<EventQueueMonitor<SQLiteWrapper>>(onEvent);
    }

    void TearDown(const ::benchmark::State& state) override
    {
        monitor.reset();
    }

    std::unique_ptr<EventQueueMonitor<SQLiteWrapper>> monitor;
    std::function<bool(const std::string&)> onEvent;
};

BENCHMARK_DEFINE_F(EventQueueMonitorFixture, DispatchPendingEvents)(benchmark::State& state)
{
    for (auto _ : state)
    {
        std::vector<Event> pending_events(state.range(0));

        for (int i = 0; i < state.range(0); ++i)
        {
            monitor->eventQueue->InsertEvent(i, "event_data", "event_type");
        }
    }
}

BENCHMARK_REGISTER_F(EventQueueMonitorFixture, DispatchPendingEvents)->Arg(10)->Arg(100)->Arg(1000);

BENCHMARK_MAIN();
