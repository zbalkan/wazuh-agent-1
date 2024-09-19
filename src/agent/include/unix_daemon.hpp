#pragma once

#include <string>
#include <unistd.h>
#include <vector>

class AgentDaemon
{
public:
    static int Daemonize();
    static bool IsAlreadyRunning();
    static void Stop();
    static std::string GetStatus();

private:
    static std::vector<pid_t> GetPidsByName(const std::string& process_name);
};
