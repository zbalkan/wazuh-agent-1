#include "command_executor.hpp"
#include <iostream>


command_executor::command_executor(){}

command_executor::~command_executor(){}

bool command_executor::execute(const nlohmann::json& command) {
    std::cout << "Executing command: " << command.dump() << std::endl;
    return true;
}