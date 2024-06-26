#ifndef COMMAND_EXECUTOR_HPP
#define COMMAND_EXECUTOR_HPP

#include <nlohmann/json.hpp>

class command_executor {

public:

    command_executor();

    ~command_executor();

    bool execute(const nlohmann::json& command);

};

#endif // COMMAND_EXECUTOR_HPP
