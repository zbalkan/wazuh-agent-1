#pragma once

#include <functional>

class ThreadManagerWrapper
{
public:
    virtual ~ThreadManagerWrapper() = default;

    virtual void CreateThreadImpl(std::function<void()> func) = 0;

    template<typename Function, typename... Args>
    void CreateThread(Function&& function, Args&&... args)
    {
        CreateThreadImpl(std::bind(std::forward<Function>(function), std::forward<Args>(args)...));
    }

    virtual void JoinAll() = 0;

    virtual void CleanUpJoinableThreads() = 0;
};
