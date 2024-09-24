#pragma once

#include <functional>
#include <string>
#include "configuration.hpp"

struct ModuleWrapper {
    std::function<void()> start;
    std::function<int(const Configuration&)> setup;
    std::function<void()> stop;
    std::function<std::string(const std::string&)> command;
    std::function<std::string()> name;
};
