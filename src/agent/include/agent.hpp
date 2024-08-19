#pragma once

#include <agent_info.hpp>
#include <command_handler.hpp>
#include <communicator.hpp>
#include <configuration_parser.hpp>
#include <isignal_handler.hpp>
#include <multitype_queue.hpp>
#include <signal_handler.hpp>
#include <task_manager.hpp>

#include <memory>

class Agent
{
public:
    Agent(std::unique_ptr<ISignalHandler> signalHandler = std::make_unique<SignalHandler>());
    ~Agent();

    void Run();

private:
    std::shared_ptr<MultiTypeQueue> m_messageQueue;

    std::unique_ptr<ISignalHandler> m_signalHandler;
    TaskManager m_taskManager;
    AgentInfo m_agentInfo;
    configuration::ConfigurationParser m_configurationParser;
    communicator::Communicator m_communicator;
    command_handler::CommandHandler m_commandHandler;
    MultiTypeQueue m_agentQueue;

    boost::asio::awaitable<void> PushTestMessages()
    {
        using namespace std::chrono_literals;
        const auto executor = co_await boost::asio::this_coro::executor;
        std::unique_ptr<boost::asio::steady_timer> expTimer = std::make_unique<boost::asio::steady_timer>(executor);

        // const nlohmann::json dataContent = {{"command", {{"name", "001"}, {"type", "stateless"}}},
        //                                    {"origin", {{"serverName", "node01"}, {"moduleName", "upgradeModule"}}},
        //                                    {"parameters", {{"error", 0}, {"data", "Command received"}}},
        //                                    {"status", "Pending"}};

        int count = 0;
        while (true)
        {
            std::string strCmdName = "upgradeModule" + std::to_string(count++);
            const nlohmann::json dataContent = {{"command", strCmdName}, {"status", "Pending"}};
            const Message messageToSend {MessageType::COMMAND, dataContent, "CommandHandler"};
            m_agentQueue.push(messageToSend);

            expTimer->expires_after(3000ms);
            co_await expTimer->async_wait(boost::asio::use_awaitable);
        }
    }
};
