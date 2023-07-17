// forked from https://github.com/progschj/ThreadPool
#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace progschj
{

class ThreadPool
{
public:
    ThreadPool(size_t);
    template <class F, class... Args>
    std::future<typename std::result_of<F(Args...)>::type> enqueue(F&& f, Args&&... args);
    ~ThreadPool();

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> workers;
    // the task queue
    std::queue<std::function<void()>> tasks;

    // synchronization
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;
};


// add new work item to the pool
template <class F, class... Args>
std::future<typename std::result_of<F(Args...)>::type> ThreadPool::enqueue(F&& f, Args&&... args)
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]() { (*task)(); });
    }
    condition.notify_one();
    return res;
}

}
