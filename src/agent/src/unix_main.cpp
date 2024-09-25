#include <cmd_ln_parser.hpp>
#include <logger.hpp>
#include <process_options.hpp>
#include <register.hpp>

#include <unix_daemon.hpp>

#include <chrono>
#include <iostream>

int main(int argc, char* argv[])
{
    Logger logger;
    CommandlineParser cmdParser(argc, argv);

    LogInfo("Starting Wazuh Agent service.");

    try
    {
        if (cmdParser.OptionExists("register"))
        {
            if (cmdParser.OptionExists("--user") && cmdParser.OptionExists("--password") &&
                cmdParser.OptionExists("--key"))
            {
                agent_registration::AgentRegistration reg(cmdParser.GetOptionValue("--user"),
                                                          cmdParser.GetOptionValue("--password"),
                                                          cmdParser.GetOptionValue("--key"),
                                                          cmdParser.GetOptionValue("--name"));

                if (reg.SendRegistration())
                {
                    LogInfo("Agent registered.");
                }
                else
                {
                    LogError("Registration fail.");
                }
            }
            else
            {
                LogError("--user, --password and --key args are mandatory");
            }
        }
        else if (cmdParser.OptionExists("start"))
        {
            if (AgentDaemon::IsAlreadyRunning())
            {
                std::cout << "Wazuh-Daemon is already running\n";
                return 0;
            }

            int ret = AgentDaemon::Daemonize();

            if (ret == 0)
            {
                LogInfo("Wazuh-Daemon started");
            }
            else
            {
                LogError("Wazuh Daemon start failed");
                return 1;
            }
            StartAgent();
        }
        else if (cmdParser.OptionExists("status"))
        {
            StatusAgent();
        }
        else if (cmdParser.OptionExists("stop"))
        {
            StopAgent();
        }
        else if (cmdParser.OptionExists("--help"))
        {
            PrintHelp();
        }
        else
        {
            PrintHelp();
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        LogCritical("An error occurred: {}.", e.what());
        return 1;
    }
}
