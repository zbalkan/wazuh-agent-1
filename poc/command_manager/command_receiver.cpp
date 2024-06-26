#include "command_receiver.hpp"
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

// Constructor
command_receiver::command_receiver(const std::string& db_path) : m_db(nullptr) {
    ce = command_executor();
    int rc = sqlite3_open(db_path.c_str(), &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_close(m_db);
        m_db = nullptr;
    } else {
        create_table_if_not_exists();
    }
}

// Destructor
command_receiver::~command_receiver() {
    if (m_db) {
        sqlite3_close(m_db);
    }
}

// Member function to read a JSON file and store it in m_json_data
void command_receiver::receive_command() {
    while (true){
        std::string json_input;
        std::cout << "Received JSON: ";
        std::getline(std::cin, json_input);

        // Convertir el std::stringstream a un objeto json
        m_json_data  = nlohmann::json::parse(json_input);

        auto timestamp = get_current_timestamp();
        insert_command(timestamp, m_json_data, "pending");
        pending_commands++;
    }
}

void command_receiver::dispatch_command() {
    while (true){
        if (pending_commands > 0){
            nlohmann::json executating_command = get_oldest_command();
            update_command(executating_command["timestamp"], "processing");
            executating_command["status"] = "processing";
            pending_commands--;
            if(ce.execute(executating_command)){
                update_command(executating_command["timestamp"], "done");
            }
            else{
                std::cout << "Failed command execution: " << executating_command << std::endl;  
            }
        }
    }
}

// Private helper function to get the current timestamp
int64_t command_receiver::get_current_timestamp() const {
    auto now_high = std::chrono::high_resolution_clock::now();
    auto duration_high = now_high.time_since_epoch();
    auto micros_high = std::chrono::duration_cast<std::chrono::microseconds>(duration_high).count();
    return micros_high;
}

// Private helper function to create the table if it doesn't exist
void command_receiver::create_table_if_not_exists() {
    const char* sql_create_table = "CREATE TABLE IF NOT EXISTS commands ("
                                   "timestamp INTEGER PRIMARY KEY, "
                                   "data JSON, "
                                   "status TEXT CHECK(status IN ('pending', 'processing', 'done')) DEFAULT 'pending');";
    char* err_msg = nullptr;
    int rc = sqlite3_exec(m_db, sql_create_table, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
}

// Member function to insert JSON data into a SQLite database
void command_receiver::insert_command(int64_t timestamp, nlohmann::json command, const std::string& status) {
    if (m_db) {
        std::string json_str = command.dump();
        const char* sql_insert = "INSERT INTO commands (timestamp, data, status) VALUES (?, ?, ?);";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(m_db, sql_insert, -1, &stmt, nullptr);
        if (rc == SQLITE_OK) {
            // Bind the parameters
            sqlite3_bind_int64(stmt, 1, timestamp);
            sqlite3_bind_text(stmt, 2, json_str.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, status.c_str(), -1, SQLITE_STATIC);
            
            // Execute the statement
            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                std::cerr << "Execution failed: " << sqlite3_errmsg(m_db) << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Failed to execute statement: " << sqlite3_errmsg(m_db) << std::endl;
        }
    }
}

void command_receiver::update_command(int64_t timestamp, const std::string& status) {

    if (m_db) {
        const char* sql_update = "UPDATE commands SET status=? WHERE timestamp=?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(m_db, sql_update, -1, &stmt, nullptr);
        if (rc == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, status.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int64(stmt, 2, timestamp);
            if (sqlite3_step(stmt) != SQLITE_DONE) {
                std::cerr << "Failed to execute update: " << sqlite3_errmsg(m_db) << std::endl;
            } else {
                std::cout << "Update successful" << std::endl;
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        }
    }
}

nlohmann::json command_receiver::get_oldest_command() {
    if (m_db) {
        sqlite3_stmt* stmt;
        const char* sql = "SELECT * FROM commands WHERE status='pending' ORDER BY timestamp ASC LIMIT 1;";
        nlohmann::json result;

        int rc = sqlite3_prepare_v2(m_db, sql, -1, &stmt, 0);
        if (rc == SQLITE_OK) {
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int64_t timestamp = sqlite3_column_int64(stmt, 0);
                const unsigned char* data = sqlite3_column_text(stmt, 1);
                const unsigned char* status = sqlite3_column_text(stmt, 2);
                result["timestamp"] = timestamp;
                result["command"] = (data ? reinterpret_cast<const char*>(data) : "NULL");
                result["status"] = (status ? reinterpret_cast<const char*>(status) : "NULL");
            }
            sqlite3_finalize(stmt);
        } else {
            std::cerr << "Failed to execute statement: " << sqlite3_errmsg(m_db) << std::endl;
        }

        return result;
    }
    return NULL;
}
