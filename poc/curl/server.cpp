#include <boost/asio/ip/tcp.hpp>
#include <chrono>
#include <ctime>
#include <string>
#include <unordered_map>
#include <thread>
#include <vector>

#include "server/listener.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

int main(int argc, char* argv[])
{
    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(8080);

    net::io_context ioc;

    std::make_shared<listener>(ioc, tcp::endpoint {address, port})->run();

    // Run the I/O service on multiple threads
    std::vector<std::thread> threads;
    auto num_threads = std::thread::hardware_concurrency();
    for (auto i = 0; i < num_threads; ++i)
    {
        threads.emplace_back([&ioc] {
            ioc.run();
        });
    }

    for (auto& t : threads)
    {
        t.join();
    }

    return 0;
}
