#ifndef VISION_AOD_FST_THREAD_POOL_HPP
#define VISION_AOD_FST_THREAD_POOL_HPP

#include <pthread.h>

#include <cassert>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace cripty_project
{

class ThreadPool
{
public:
    explicit ThreadPool(unsigned int numThreads, const std::string& pool_name = "tp");

    // No copy
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    ~ThreadPool();

    void Stop();

    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
    {
        using result_type = std::invoke_result_t<F, Args...>; //Deduces the return type of an INVOKE expression at compile time.

        // Ensure that m_threads is not empty
        if (m_threads.empty())
        {
            return std::future<result_type>();
        }
        std::packaged_task<std::remove_reference_t<result_type>()> //std::packaged_task requires a non-reference type, see below explanation
            task([=]() mutable {
                return std::invoke(std::forward<F>(f), std::forward<Args>(args)...); // will invoke the function with the arguments when task be called
            });

        auto result = task.get_future();
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            if (m_stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");
            m_tasks.emplace(std::move(task));
        }
        m_condition.notify_one();
        return result;
    }

private:
    std::vector<std::thread>               m_threads;
    std::queue<std::packaged_task<void()>> m_tasks;

    std::mutex              m_queue_mutex;
    std::condition_variable m_condition;
    std::atomic<bool>       m_stop{false};

    void StartThreadPool(unsigned int numThreads, const std::string& pool_name);
};

} // namespace cripty_project


/*
 * std::packaged_task is a class template that represents a packaged task that can be executed asynchronously. The template parameter of std::packaged_task is the type of the function or callable object that will be executed.

According to the C++ standard (§30.3.3.1.2), the template parameter of std::packaged_task must be a copyable and movable object type that is callable with zero or more arguments. Specifically, it says:

The template parameter type F of std::packaged_task shall meet the requirements of Callable (20.14) for zero or more arguments.

Now, the requirements for a Callable object are specified in §20.14.2 of the C++ standard, which states:

A type F is Callable if it can be invoked with a set of arguments Args. [...] The return type of a Callable object is determined as follows:

If the expression INVOKE<R>(f, t1, t2, ..., tN) is well-formed when treated as an unevaluated operand (17.6.5.2), the return type shall be R.
Otherwise, if the expression INVOKE(f, t1, t2, ..., tN) is well-formed when treated as an unevaluated operand, the return type shall be decltype(INVOKE(f, t1, t2, ..., tN)).
Otherwise, the program is ill-formed.
So, according to the requirements for Callable objects, the return type of a Callable object is determined by the INVOKE expression. Since std::packaged_task requires a copyable and movable object type, and since references cannot be copied or moved, the return type of the Callable object passed to std::packaged_task must not have any reference qualifiers.

Therefore, when constructing a std::packaged_task, any reference qualifiers on the return type of the Callable object must be removed using std::remove_reference to ensure that the resulting type is a copyable and movable object type.
 *
 */

#endif //VISION_AOD_FST_THREAD_POOL_HPP
