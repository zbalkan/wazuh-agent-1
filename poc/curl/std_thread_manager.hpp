#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <vector>

#include "thread_manager_wrapper.hpp"

class StdThreadManager : public ThreadManagerWrapper
{
private:
    std::mutex threads_mutex;
    std::vector<std::thread> threads;

public:

    void CreateThreadImpl(std::function<void()> func) override {
        std::lock_guard<std::mutex> lock(threads_mutex);
        threads.emplace_back(std::move(func));
    }

    void CleanUpJoinableThreads() override
    {
        std::lock_guard<std::mutex> lock(threads_mutex);
        auto it = std::remove_if(threads.begin(),
                                 threads.end(),
                                 [](std::thread& t)
                                 {
                                     if (t.joinable())
                                     {
                                         t.join();
                                         return true;
                                     }
                                     return false;
                                 });

        if (it != threads.end())
        {
            threads.erase(it, threads.end());
        }
    }

    void JoinAll() override
    {
        std::lock_guard<std::mutex> lock(threads_mutex);
        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }
};
