#pragma once

#include <functional>
#include <thread>
#include <vector>

#include "thread_manager_wrapper.hpp"

class StdThreadManager : public ThreadManagerWrapper
{
private:
    std::vector<std::thread> threads;

public:

    void CreateThreadImpl(std::function<void()> func) override {
        threads.emplace_back(std::move(func));
    }

    void CleanUpJoinableThreads() override
    {
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

        threads.erase(it, threads.end());
    }

    void JoinAll() override
    {
        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }
};
