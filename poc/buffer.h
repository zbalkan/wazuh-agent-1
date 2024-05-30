#pragma once

#include <condition_variable>
#include <vector>
#include <mutex>
#include <string>
#include <vector>


class CircularBuffer
{
public:
    CircularBuffer(size_t size)
    : buffer_(size)
    , head_(0)
    , tail_(0)
    , full_(false)
    {
    }

    void put(const std::string& item)
    {
        buffer_[head_] = item;
        head_ = (head_ + 1) % buffer_.size();
        if (full_) {
            tail_ = (tail_ + 1) % buffer_.size();
        }
        full_ = head_ == tail_;
    }

    std::string get()
    {
        auto item = buffer_[tail_];
        full_ = false;
        tail_ = (tail_ + 1) % buffer_.size();
        return item;
    }

    bool empty() const
    {
        return (!full_ && (head_ == tail_));
    }

private:
    std::vector<std::string> buffer_;
    size_t head_, tail_;
    bool full_;
};
