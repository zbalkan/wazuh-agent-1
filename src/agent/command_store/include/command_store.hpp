#pragma once

#include <string>
#include <vector>

namespace command_store
{
    class CommandStore
    {
    private:
        std::vector<std::string> m_vCommands;
    public:
        CommandStore();
        ~CommandStore() {}

        void StoreCommand(const std::string& command);
        void DeleteCommand(int id);

    };
} // namespace command_store
