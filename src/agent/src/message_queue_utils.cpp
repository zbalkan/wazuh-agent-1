#include <imultitype_queue.hpp>
#include <message_queue_utils.hpp>
#include <nlohmann/json.hpp>

#include <vector>

namespace
{
    // This should eventually be replaced with a configuration parameter.
    constexpr int NUM_EVENTS = 1;
} // namespace

boost::asio::awaitable<std::string> GetMessagesFromQueue(std::shared_ptr<IMultiTypeQueue> multiTypeQueue,
                                                         MessageType messageType)
{
    const auto message = co_await multiTypeQueue->getNextNAwaitable(messageType, NUM_EVENTS);

    nlohmann::json jsonObj;
    jsonObj["events"] = message.data;

    co_return jsonObj.dump();
}

void PopMessagesFromQueue(std::shared_ptr<IMultiTypeQueue> multiTypeQueue, MessageType messageType)
{
    multiTypeQueue->popN(messageType, NUM_EVENTS);
}

void PushCommandsToQueue(std::shared_ptr<IMultiTypeQueue> multiTypeQueue, const std::string& commands)
{
    const auto jsonObj = nlohmann::json::parse(commands);

    if (jsonObj.contains("commands") && jsonObj["commands"].is_array())
    {
        std::vector<Message> messages;

        for (const auto& command : jsonObj["commands"])
        {
            messages.emplace_back(MessageType::COMMAND, command);
        }

        if (!messages.empty())
        {
            multiTypeQueue->push(messages);
        }
    }
}

std::optional<command_store::CommandEntry> GetCommandFromQueue(std::shared_ptr<IMultiTypeQueue> multiTypeQueue)
{
    if (multiTypeQueue->isEmpty(MessageType::COMMAND))
    {
        return std::nullopt;
    }

    Message m = multiTypeQueue->getNext(MessageType::COMMAND);
    nlohmann::json jsonData = m.data.at(0).at("data");

    std::string id;
    std::string module;
    std::string command;
    std::string parameters;

    if (jsonData.contains("id") && jsonData["id"].is_string())
    {
        id = jsonData["id"].get<std::string>();
    }

    if (jsonData.contains("args") && jsonData["args"].is_array())
    {
        int index = 0;
        for (const auto& arg : jsonData["args"])
        {
            switch (index++)
            {
                case 0:
                    if (arg.is_string())
                        module = arg.get<std::string>();
                    break;
                case 1:
                    if (arg.is_string())
                        command = arg.get<std::string>();
                    break;
                default:
                    if (!parameters.empty())
                        parameters += " ";
                    parameters += arg.is_string() ? arg.get<std::string>() : arg.dump();
                    break;
            }
        }
    }

    command_store::CommandEntry cmd(id, module, command, parameters, "", command_store::Status::IN_PROGRESS);

    return cmd;
}

void PopCommandFromQueue(std::shared_ptr<IMultiTypeQueue> multiTypeQueue)
{
    multiTypeQueue->pop(MessageType::COMMAND);
}
