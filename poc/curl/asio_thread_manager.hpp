#include "thread_manager_wrapper.hpp"

#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <functional>
#include <memory>

class AsioThreadManager : public ThreadManagerWrapper
{
public:
    AsioThreadManager(std::size_t num_threads = 32)
        : thread_pool_(num_threads)
    {
    }

    ~AsioThreadManager()
    {
        thread_pool_.join();
    }

    void CreateThreadImpl(std::function<void()> func) override
    {
        boost::asio::post(thread_pool_, std::move(func));
    }

    void JoinAll() override
    {
        thread_pool_.join();
    }

    void CleanUpJoinableThreads() override
    {
        // No need for explicit cleanup as boost::asio::thread_pool handles this
    }

private:
    boost::asio::thread_pool thread_pool_;
};
