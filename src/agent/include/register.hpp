#pragma once

#include <agent_info.hpp>
#include <configuration_parser.hpp>

#include <string>

namespace agent_registration
{
    struct UserCredentials
    {
        std::string user;
        std::string password;
    };

    class AgentRegistration
    {
    public:
        AgentRegistration(std::string user, std::string password, const std::string& key, const std::string& name);
        bool SendRegistration();

    private:
        configuration::ConfigurationParser m_configurationParser;
        std::string m_managerIp;
        std::string m_managerPort;
        std::string m_user;
        std::string m_password;
        AgentInfo m_agentInfo;
    };
} // namespace agent_registration
