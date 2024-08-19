#include <agent.hpp>

#include <http_client.hpp>
#include <message.hpp>
#include <message_queue_utils.hpp>
#include <signal_handler.hpp>

#include <memory>
#include <string>
#include <thread>

Agent::Agent(std::unique_ptr<ISignalHandler> signalHandler)
    : m_messageQueue(std::make_shared<MultiTypeQueue>())
    , m_signalHandler(std::move(signalHandler))
    , m_communicator(std::make_unique<http_client::HttpClient>(),
                     m_agentInfo.GetUUID(),
                     m_agentInfo.GetKey(),
                     [this](std::string table, std::string key) -> std::string
                     { return m_configurationParser.GetConfig<std::string>(std::move(table), std::move(key)); })
{
    m_taskManager.Start(std::thread::hardware_concurrency());
}

Agent::~Agent()
{
    m_taskManager.Stop();
}

void Agent::Run()
{
    m_taskManager.EnqueueTask(m_communicator.WaitForTokenExpirationAndAuthenticate());

    m_taskManager.EnqueueTask(m_communicator.GetCommandsFromManager(
        [this](const std::string& response) { PushCommandsToQueue(m_messageQueue, response); }));

    m_taskManager.EnqueueTask(m_communicator.StatefulMessageProcessingTask(
        [this]() { return GetMessagesFromQueue(m_messageQueue, MessageType::STATEFUL); },
        [this]([[maybe_unused]] const std::string& response)
        { PopMessagesFromQueue(m_messageQueue, MessageType::STATEFUL); }));

    m_taskManager.EnqueueTask(m_communicator.StatelessMessageProcessingTask(
        [this]() { return GetMessagesFromQueue(m_messageQueue, MessageType::STATELESS); },
        [this]([[maybe_unused]] const std::string& response)
        { PopMessagesFromQueue(m_messageQueue, MessageType::STATELESS); }));

    m_taskManager.EnqueueTask(PushTestMessages());

    m_taskManager.EnqueueTask(m_commandHandler.ProcessCommandsFromQueue<Message>(
        [this]() -> std::optional<Message>
        {
            while (m_agentQueue.isEmpty(MessageType::COMMAND))
            {
                return std::nullopt;
            }

            Message m = m_agentQueue.getNext(MessageType::COMMAND);

            std::cout << "--------- COMMAND retrieved from Queue:" << std::endl;
            std::cout << m.data.dump() << std::endl;

            // pop message from queue
            m_agentQueue.pop(MessageType::COMMAND);

            // change status and push to CommandStore
            Message newMessage(MessageType::COMMAND, m.data, "CommandHandler");
            setJsonValue(newMessage.data, "status", "InProgress");
            std::cout << newMessage.data.dump() << std::endl;
            // m_agentQueue.push(newMessage);

            return m;
        },
        [this](Message& cmd) -> int
        {
            std::cout << "Dispatching command" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            return 1;
        }));

    m_signalHandler.WaitForSignal();
}
