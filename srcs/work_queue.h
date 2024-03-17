#ifndef WORK_QUEUE_H
#define WORK_QUEUE_H

#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <thread>
#include <future>
#include <atomic>

template <typename T>
class WorkQueue
{
public:
    WorkQueue();

    WorkQueue(const size_t max_num_works);

    size_t Size();

    bool Empty();

    bool Push(T data);

    T Pop();

    void WaitTillEmpty();

    void Stop();

    void Clear();

private:
    std::queue<T> work_queue_;
    size_t max_num_works;
    std::mutex mutex_;
    std::atomic<bool> stop_;

    std::condition_variable push_condition_;
    std::condition_variable pop_condition_;
    std::condition_variable empty_condition_;
};

template <typename T>
WorkQueue<T>::WorkQueue()
{
    this->max_num_works = -1; //Default to set no restrictions
}

template <typename T>
WorkQueue<T>::WorkQueue(const size_t max_num_works)
{
    this->max_num_works = max_num_works;
}

template <typename T>
bool WorkQueue<T>::Push(T data)
{
    std::unique_lock<std::mutex> lock(mutex_);
    while(max_num_works != -1 && work_queue_.size() >= max_num_works && !stop_)
    {
        pop_condition_.wait(lock);
    }
    if(stop_)
    {
        return false;
    }
    work_queue_.push(std::move(data));
    push_condition_.notify_one();
    return true;
}

template <typename T>
T WorkQueue<T>::Pop()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while(work_queue_.empty() && !stop_)
    {
        push_condition_.wait(lock);
    }
    if(stop_)
    {
        return T();
    }
    T data = std::move(work_queue_.front());
    work_queue_.pop();
    pop_condition_.notify_one();
    if(work_queue_.empty())
    {
        empty_condition_.notify_all();
    }
    return data;
}

template <typename T>
size_t WorkQueue<T>::Size()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return work_queue_.size();
}

template <typename T>
void WorkQueue<T>::WaitTillEmpty()
{
    std::unique_lock<std::mutex> lock(mutex_);
    while(!work_queue_.empty())
    {
        empty_condition_.wait(lock);
    }
}

template <typename T>
void WorkQueue<T>::Stop()
{
    stop_ = true;
    push_condition_.notify_all();
    pop_condition_.notify_all();
}

template <typename T>
void WorkQueue<T>::Clear()
{
    std::unique_lock<std::mutex> lock(mutex_);
    std::queue<T> empty_queues;
    std::swap(work_queue_, empty_queues);
    empty_condition_.notify_all();
}

template <typename T>
bool WorkQueue<T>::Empty()
{
    std::unique_lock<std::mutex> lock(mutex_);
    bool emp = work_queue_.empty();
    if(emp) empty_condition_.notify_all();
    return emp;
}
#endif