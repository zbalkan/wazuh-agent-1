#include <agent.hpp>
#include <logger.hpp>
#include <process_options.hpp>

#include <iostream>

void StartAgent()
{
    LogInfo("Starting Wazuh Agent.");

    Agent agent;
    agent.Run();
}

void StatusAgent()
{
    std::string status = "running";
    LogInfo("Wazuh Agent is {}.", status);

    // TO DO
}

void StopAgent()
{
    LogInfo("Stopping Wazuh Agent.");

    // TO DO
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
