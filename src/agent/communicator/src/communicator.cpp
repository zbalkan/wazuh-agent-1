#include <communicator.hpp>

#include <http_client.hpp>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <queue>
#include <thread>

namespace communicator
{
    constexpr int TokenPreExpirySecs = 2;

    Communicator::Communicator(const std::string& uuid,
                               const std::function<std::string(std::string, std::string)> GetStringConfigValue)
        : m_exitFlag(false)
        , m_tokenExpTimeInSeconds(0)
        , m_uuid(uuid)
    {
        if (GetStringConfigValue != nullptr)
        {
            m_managerIp = GetStringConfigValue("agent", "manager_ip");
            m_port = GetStringConfigValue("agent", "port");
        }
    }

    Communicator::~Communicator()
    {
        Stop();
    }

    boost::beast::http::status Communicator::SendAuthenticationRequest()
    {
        const nlohmann::json bodyJson = {{"uuid", m_uuid}};
        const auto reqParams = http_client::HttpRequestParams(
            boost::beast::http::verb::post, m_managerIp, m_port, "/authentication", "", "", bodyJson.dump());
        const auto res = http_client::SendHttpRequest(reqParams);

        if (res.result() != boost::beast::http::status::ok)
        {
            return res.result();
        }

        m_token = boost::beast::buffers_to_string(res.body().data());

        if (const auto decoded = jwt::decode(m_token); decoded.has_payload_claim("exp"))
        {
            const auto exp_claim = decoded.get_payload_claim("exp");
            const auto exp_time = exp_claim.as_date();
            m_tokenExpTimeInSeconds =
                std::chrono::duration_cast<std::chrono::seconds>(exp_time.time_since_epoch()).count();
        }
        else
        {
            std::cerr << "Token does not contain an 'exp' claim" << std::endl;
        }

        return res.result();
    }

    long Communicator::GetTokenRemainingSecs() const
    {
        const auto now = std::chrono::system_clock::now();
        const auto now_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        return std::max(0L, static_cast<long>(m_tokenExpTimeInSeconds - now_seconds));
    }

    void Communicator::Stop()
    {
        std::lock_guard<std::mutex> lock(m_exitMtx);
        m_exitFlag = true;
    }

    boost::asio::awaitable<void> Communicator::GetCommandsFromManager()
    {
        const auto reqParams =
            http_client::HttpRequestParams(boost::beast::http::verb::get, m_managerIp, m_port, "/commands");
        co_await http_client::Co_MessageProcessingTask(m_token, reqParams, {});
    }

    boost::asio::awaitable<void> Communicator::WaitForTokenExpirationAndAuthenticate()
    {
        using namespace std::chrono_literals;
        const auto executor = co_await boost::asio::this_coro::executor;
        boost::asio::steady_timer timer(executor);

        while (true)
        {
            const auto duration = [this]()
            {
                const auto result = SendAuthenticationRequest();
                if (result != boost::beast::http::status::ok)
                {
                    std::cerr << "Authentication failed." << std::endl;
                    return std::chrono::milliseconds(1000);
                }
                else
                {
                    return std::chrono::milliseconds((GetTokenRemainingSecs() - TokenPreExpirySecs) * 1000);
                }
            }();

            timer.expires_after(duration);
            co_await timer.async_wait(boost::asio::use_awaitable);

            {
                std::lock_guard<std::mutex> lock(m_exitMtx);
                if (m_exitFlag)
                    co_return;
            }
        }
    }

    boost::asio::awaitable<void> Communicator::StatefulMessageProcessingTask(std::queue<std::string>& messageQueue)
    {
        const auto reqParams =
            http_client::HttpRequestParams(boost::beast::http::verb::post, m_managerIp, m_port, "/stateful");
        co_await http_client::Co_MessageProcessingTask(m_token, reqParams, {});
    }

    boost::asio::awaitable<void> Communicator::StatelessMessageProcessingTask(std::queue<std::string>& messageQueue)
    {
        const auto reqParams =
            http_client::HttpRequestParams(boost::beast::http::verb::post, m_managerIp, m_port, "/stateless");
        co_await http_client::Co_MessageProcessingTask(m_token, reqParams, {});
    }
} // namespace communicator
