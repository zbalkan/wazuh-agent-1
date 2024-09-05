#pragma once

#include <string>

namespace command_store
{
    enum class Status
    {
        SUCCESS,
        ERROR,
        INPROCESS,
        TIMEOUT
    };

    class Command
    {
    public:
        Command() {}

        Command(
            int id, const std::string& module, const std::string& command, const std::string& parameters, Status status)
            : m_id(id)
            , m_module(module)
            , m_command(command)
            , m_parameters(parameters)
            , m_status(status)
        {
        }

        int m_id;
        std::string m_module;
        std::string m_command;
        std::string m_parameters;
        Status m_status;
        double m_time;
    };
} // namespace command_store
