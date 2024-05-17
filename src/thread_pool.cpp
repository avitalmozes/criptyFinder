#include "thread_pool.hpp"

namespace cripty_project
{



ThreadPool::ThreadPool(unsigned int numThreads, const std::string& pool_name)
{
    m_threads.reserve(numThreads);
    StartThreadPool(numThreads, pool_name);
}
ThreadPool::~ThreadPool()
{
    Stop();
}
void ThreadPool::Stop()
{
    m_stop = true;
    m_condition.notify_all();
    for (auto& worker : m_threads)
    {
        worker.join();
    }
    m_threads.clear();
}

void ThreadPool::StartThreadPool(unsigned int numThreads, const std::string& pool_name)
{
    for (unsigned int i = 0; i < numThreads; ++i)
    {
        m_threads.emplace_back([this] 
        {
            while (true)
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_condition.wait(lock, [this] 
                    { return m_stop || !m_tasks.empty(); });
                if (m_stop && m_tasks.empty())
                    return;
                auto task = std::move(m_tasks.front());
                m_tasks.pop();
                lock.unlock();
                if (task.valid())
                {
                    task();
                }
            }
        });
        std::string thread_name = pool_name + std::to_string(i);
        pthread_setname_np(m_threads[i].native_handle(), thread_name.c_str());
    }
}

} // namespace cripty_project
