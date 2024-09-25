#include <process_options.hpp>

#include <agent.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <logger.hpp>
#include <unix_daemon.hpp>

#include <iostream>
#include <vector>

void StartAgent()
{
    LogInfo("Starting Wazuh Agent.");

    Agent agent;
    agent.Run();
}

void StatusAgent()
{
    std::string status = fmt::format("Wazuh Agent is {}\n", AgentDaemon::GetStatus());
    std::cout << status;
}

void StopAgent()
{
    LogInfo("Stopping Wazuh Agent.");

    AgentDaemon::Stop();
}

void PrintHelp()
{
    LogInfo("Wazuh Agent help.");

    // TO DO
    std::cout << "wazuh-agent [start/status/stop]\n";
    std::cout << "     start    Start wazuh-agent daemon\n";
    std::cout << "     status   Get wazuh-agent daemon status\n";
    std::cout << "     stop     Stop wazuh-agent daemon\n";
    std::cout << "     --h      This help message\n";
}
