#pragma once

#include <string>
#include <vector>

struct Event{
    int id;
    std::string event_data;
    std::string event_type;
    std::string status;
};

struct Command{
    int id;
    std::string command_data;
    std::string status;
};

template <typename DB>
class DBWrapper {
public:
    virtual ~DBWrapper() = default;

    virtual void createTable() = 0;
    virtual void insertEvent(int id, const std::string& event_data, const std::string& event_type) = 0;
    virtual std::vector<Event> fetchPendingEvents(int limit) = 0;
    virtual void updateEventStatus(const std::vector<int>& event_ids) = 0;
    virtual int getPendingEventCount() = 0;
    virtual void insertCommand(const std::string& command_data) = 0;
    virtual Command fetchPendingCommand() = 0;
    virtual void updateCommandStatus(int commandId) = 0;
};
