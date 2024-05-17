#ifndef VISION_AOD_FST_THREAD_POOL_HPP
#define VISION_AOD_FST_THREAD_POOL_HPP

#include <functional>
#include <future>
#include <queue>

namespace crypty_project
{

/**
 * @brief ThreadPool class for managing a pool of threads.
 */
class ThreadPool
{
public:

    /**
     * @brief Constructs a ThreadPool with the specified number of threads.
     * 
     * @param num_threads The number of threads in the pool.
     * @param pool_name The name of the thread pool.
     */
    explicit ThreadPool(unsigned int num_threads, const std::string& pool_name = "tp");

    // No copy
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    /**
     * @brief Destructor for ThreadPool class.
     */
    ~ThreadPool();

    /**
     * @brief Stops the ThreadPool and joins all threads.
     */
    void Stop();

    /**
     * @brief Enqueues a task to be executed by the ThreadPool.
     * 
     * @tparam F Type of the callable object.
     * @tparam Args Variadic template parameters for the arguments of the callable object.
     * @param f Callable object to be executed.
     * @param args Arguments to be passed to the callable object.
     * @return A future representing the result of the task.
     */
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
    std::vector<std::thread>               m_threads;       /**< Vector of threads in the ThreadPool. */
    std::queue<std::packaged_task<void()>> m_tasks;         /**< Queue of tasks to be executed. */

    std::mutex              m_queue_mutex;    /**< Mutex for protecting the task queue. */
    std::condition_variable m_condition;      /**< Condition variable for task synchronization. */
    std::atomic<bool>       m_stop{false};    /**< Atomic boolean flag to indicate ThreadPool termination. */


    /**
     * @brief Starts the ThreadPool with the specified number of threads.
     * 
     * @param num_threads The number of threads in the pool.
     * @param pool_name The name of the thread pool.
     */
    void StartThreadPool(unsigned int num_threads, const std::string& pool_name);
};

} // namespace crypty_project

#endif //VISION_AOD_FST_THREAD_POOL_HPP
