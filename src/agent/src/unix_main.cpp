#include <cmd_ln_parser.hpp>
#include <logger.hpp>
#include <process_options.hpp>
#include <register.hpp>
#include <unix_daemon.hpp>

#include <chrono>
#include <iostream>
#include <syslog.h>

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
