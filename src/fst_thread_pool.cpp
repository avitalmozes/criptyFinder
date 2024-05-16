//
// Created by liors on 2/7/23.
//

#include "foresight/common/fst_thread_pool.hpp"

FstThreadPool::FstThreadPool(unsigned int numThreads, const std::string& pool_name)
{
    m_threads.reserve(numThreads);
    StartThreadPool(numThreads, pool_name);
}
FstThreadPool::~FstThreadPool()
{

    Stop();
}
void FstThreadPool::Stop()
{

    m_stop = true;
    m_condition.notify_all();
    for (auto& worker : m_threads)
    {
        worker.join();
    }
    m_threads.clear();
}

void FstThreadPool::StartThreadPool(unsigned int numThreads, const std::string& pool_name)
{
    for (unsigned int i = 0; i < numThreads; ++i)
    {
        m_threads.emplace_back([this] {
            for (;;)
            {

                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_condition.wait(lock, [this] { return m_stop || !m_tasks.empty(); });
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
