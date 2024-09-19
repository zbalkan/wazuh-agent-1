#include <unix_daemon.hpp>

#include <logger.hpp>

#include <algorithm>
#include <errno.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <signal.h>

namespace fs = std::filesystem;

namespace
{
    constexpr const std::string AGENT_FILENAME = "wazuh-agent";
    constexpr const char* PID_PATH = "var/run";
} // namespace

std::vector<pid_t> AgentDaemon::GetPidsByName(const std::string& process_name)
{
    std::vector<pid_t> pids;

    for (const auto& entry : fs::directory_iterator("/proc"))
    {
        if (entry.is_directory())
        {
            std::string pid_str = entry.path().filename().string();
            if (std::all_of(pid_str.begin(), pid_str.end(), ::isdigit))
            {
                std::ifstream comm_file(entry.path() / "comm");
                std::string comm;
                std::getline(comm_file, comm);
                if (comm == process_name)
                {
                    pids.push_back(std::stoi(pid_str));
                }
            }
        }
    }

    return pids;
}

int AgentDaemon::Daemonize()
{
    return daemon(1, 0);
}

bool AgentDaemon::IsAlreadyRunning()
{
    std::vector<pid_t> pids = GetPidsByName(AGENT_FILENAME);

    pid_t thisProcessPid = getpid();

    for (pid_t pid : pids)
    {
        if (pid != thisProcessPid)
        {
            return true;
        }
    }
    return false;
}

std::string AgentDaemon::GetStatus()
{
    std::vector<pid_t> pids = GetPidsByName(AGENT_FILENAME);

    pid_t thisProcessPid = getpid();

    for (pid_t pid : pids)
    {
        if (pid != thisProcessPid)
        {
            if (kill(pid, 0) == 0)
            {
                return "running";
            }
        }
    }
    return "stopped";
}

void AgentDaemon::Stop()
{
    std::vector<pid_t> pids = GetPidsByName(AGENT_FILENAME);

    pid_t thisProcessPid = getpid();

    for (pid_t pid : pids)
    {
        if (pid != thisProcessPid)
        {
            if (kill(pid, SIGTERM) == 0)
            {
                LogInfo("Terminated process with PID: {}", pid);
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
        }
    }
}
