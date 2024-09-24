#include <agent.hpp>
#include <file_utils.hpp>
#include <logger.hpp>
#include <process_options.hpp>

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
    std::string status = "running";
    LogInfo("Wazuh Agent is {}.", status);

    // TO DO
}

void StopAgent()
{
    LogInfo("Stopping Wazuh Agent.");

    PIDFileUtils pidFileUtils;

    std::vector<pid_t> pids = pidFileUtils.GetPIDFiles();

    for (const auto& pid : pids)
    {
        if (kill(pid, SIGTERM) == 0)
        {
            std::cout << "Terminated process with PID: " << pid << std::endl;
        }
        else
        {
            std::cerr << "Failed to terminate process with PID: " << pid << std::endl;
            if (kill(pid, SIGKILL) == 0)
            {
                std::cout << "Killed process with PID: " << pid << std::endl;
            }
            else
            {
                std::cerr << "Failed to kill process with PID: " << pid << std::endl;
            }
        }
        pidFileUtils.DeletePIDFile(pid);
    }
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
