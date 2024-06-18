#pragma ocne

#include "db_wrapper.hpp"

#include <iostream>
#include <atomic>
#include <vector>
#include <algorithm>

class DummyWrapper : public DBWrapper<int>
{
public:
    DummyWrapper()
    {
        events.reserve(1000);

        for (int i = 0; i < 1000; i++)
        {
            events.emplace_back(Event {i, "event_data", "event_type", "pending"});
        }
    }

    ~DummyWrapper() override
    {
    }

    void CreateTable() override
    {
    }

    void InsertEvent(int id, const std::string& event_data, const std::string& event_type) override
    {
        pending++;
    }

    std::vector<Event> FetchPendingEvents(int limit) override
    {
        limit = std::min(limit, pending.load());
        pending -= limit;
        std::vector<Event> result(events.begin(), events.begin() + limit);
        return result;
    }

    std::vector<Event> FetchAndMarkPendingEvents(int limit) override
    {
        limit = std::min(limit, pending.load());
        pending -= limit;
        processing += limit;

        std::vector<Event> result(events.begin(), events.begin() + limit);
        return result;
    }

    void UpdateEventStatus(const std::vector<int>& event_ids, const std::string& status) override
    {
        if (status == "dispatched")
        {
            processing -= event_ids.size();
            dispatched += event_ids.size();
        }
        if (status == "processing")
        {
            pending -= event_ids.size();
            processing += event_ids.size();
        }
        if (status == "pending")
        {
            processing -= event_ids.size();
            pending += event_ids.size();
        }
    }

    void DeleteEntriesWithStatus(const std::string& status) override
    {
        if (status == "dispatched")
        {
            dispatched = 0;
        }
        if (status == "processing")
        {
            processing = 0;
        }
        if (status == "pending")
        {
            pending = 0;
        }
    }

    void UpdateEntriesStatus(const std::string& from_status, const std::string& to_status) override
    {
        if (from_status == "dispatched" && to_status == "processing")
        {
            processing += dispatched;
            dispatched = 0;
        }
        if (from_status == "dispatched" && to_status == "pending")
        {
            pending += dispatched;
            dispatched = 0;
        }
        if (from_status == "processing" && to_status == "pending")
        {
            pending += processing;
            processing = 0;
        }
        if (from_status == "processing" && to_status == "dispatched")
        {
            dispatched += processing;
            processing = 0;
        }
        if (from_status == "pending" && to_status == "processing")
        {
            processing += pending;
            pending = 0;
        }

        if (from_status == "pending" && to_status == "dispatched")
        {
            dispatched += pending;
            pending = 0;
        }
    }

    int GetPendingEventCount() override
    {
        return pending;
    }

    void InsertCommand(const std::string& command_data) override {}

    Command FetchPendingCommand()
    {
        Command command;
        return command;
    }

    void UpdateCommandStatus(int commandId) {}

private:
    std::atomic<int> pending = 0;
    std::atomic<int> processing = 0;
    std::atomic<int> dispatched = 0;
    std::vector<Event> events;
};
