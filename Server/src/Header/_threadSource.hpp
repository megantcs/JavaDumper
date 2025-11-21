#pragma once

#include <thread>
#include <functional>
#include <memory>

template<typename ThreadType>
class ThreadWrapper 
{
public:
    ThreadWrapper() = default;
    ThreadWrapper(ThreadWrapper&& other) noexcept;

    ThreadWrapper(const ThreadWrapper&) = delete;
    ~ThreadWrapper();

    ThreadWrapper& operator=(const ThreadWrapper&) = delete;    
    ThreadWrapper& operator=(ThreadWrapper&& other) noexcept;
    
    explicit ThreadWrapper(std::function<void()> function);
    
    void join();
    void detach();
    bool joinable() const noexcept;
    
    void destroy();

private:
    ThreadType* thread_{nullptr};
};

template<typename ThreadType>
ThreadWrapper<ThreadType>::ThreadWrapper(ThreadWrapper&& other) noexcept
    : thread_(other.thread_)
{
    other.thread_ = nullptr;
}

template<typename ThreadType>
ThreadWrapper<ThreadType>& ThreadWrapper<ThreadType>::operator=(ThreadWrapper&& other) noexcept
{
    if (this != &other) {
        destroy();
        thread_ = other.thread_;
        other.thread_ = nullptr;
    }
    return *this;
}

template<typename ThreadType>
ThreadWrapper<ThreadType>::ThreadWrapper(std::function<void()> function)
    : thread_(new ThreadType(std::move(function)))
{
}

template<typename ThreadType>
ThreadWrapper<ThreadType>::~ThreadWrapper()
{
    destroy();
}

template<typename ThreadType>
void ThreadWrapper<ThreadType>::destroy()
{
    if (thread_ != nullptr) {
        delete thread_;
        thread_ = nullptr;
    }
}

template<typename ThreadType>
void ThreadWrapper<ThreadType>::join()
{
    if (thread_ != nullptr && thread_->joinable()) {
        thread_->join();
    }
}

template<typename ThreadType>
void ThreadWrapper<ThreadType>::detach()
{
    if (thread_ != nullptr) {
        thread_->detach();
    }
}

template<typename ThreadType>
bool ThreadWrapper<ThreadType>::joinable() const noexcept
{
    return thread_ != nullptr && thread_->joinable();
}