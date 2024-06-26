#ifndef COMMAND_RECEIVER_HPP
#define COMMAND_RECEIVER_HPP

#include <string>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include "command_executor.hpp"

class command_receiver {
public:
    // Constructor
    command_receiver(const std::string& db_path);

    // Destructor
    ~command_receiver();

    // Public member functions
    void receive_command();
    void dispatch_command();

private:
    // Private member variables
    nlohmann::json m_json_data;
    sqlite3* m_db;
    command_executor ce;
    int pending_commands = 0;

    // Private helper functions
    int64_t get_current_timestamp() const;
    void create_table_if_not_exists();
    void insert_command(int64_t timestamp, nlohmann::json command, const std::string& status);
    void update_command(int64_t timestamp, const std::string& status);
    nlohmann::json get_oldest_command();
};

#endif // COMMAND_RECEIVER_HPP
