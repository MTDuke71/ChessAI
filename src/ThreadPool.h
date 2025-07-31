#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <future>
#include <functional>
#include <mutex>
#include <condition_variable>

class ThreadPool {
public:
    explicit ThreadPool(size_t threads = std::thread::hardware_concurrency());
    ~ThreadPool();

    template<class F>
    auto enqueue(F&& f) -> std::future<typename std::invoke_result_t<F>>;

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop = false;
};

inline ThreadPool::ThreadPool(size_t threads) {
    if (threads == 0) threads = 1;
    for (size_t i = 0; i < threads; ++i) {
        workers.emplace_back([this]() {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    cv.wait(lock, [this] { return stop || !tasks.empty(); });
                    if (stop && tasks.empty()) return;
                    task = std::move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }
}

inline ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto& w : workers) w.join();
}

template<class F>
auto ThreadPool::enqueue(F&& f) -> std::future<typename std::invoke_result_t<F>> {
    using Ret = typename std::invoke_result_t<F>;
    auto task = std::make_shared<std::packaged_task<Ret()>>(std::forward<F>(f));
    std::future<Ret> res = task->get_future();
    {
        std::lock_guard<std::mutex> lock(mtx);
        tasks.emplace([task]() { (*task)(); });
    }
    cv.notify_one();
    return res;
}
