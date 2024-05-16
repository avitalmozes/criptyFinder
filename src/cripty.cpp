#include "cripty.hpp"
#include "thread_pool.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace cripty_project
{

static constexpr size_t NUM_THREADS = 20;
static std::mutex sg_cout_mutex;

CriptyInfectedFilesFinder::CriptyInfectedFilesFinder()
{
    m_thread_pool = std::make_unique<ThreadPool>(NUM_THREADS);
}

CriptyInfectedFilesFinder::~CriptyInfectedFilesFinder()
{
}

void CriptyInfectedFilesFinder::searchInfectedFiles(const std::string &root_dir, const std::string &signature)
{
    for (const auto& entry : fs::recursive_directory_iterator(root_dir)) 
    {
        std::string file_path = entry.path().string();

        if(!fs::is_directory(file_path))
        {
            m_thread_pool->enqueue(printIfInfected, file_path, signature);                
        }
    }
}

void printIfInfected(const std::string &file_path, const std::string &signature)
{
    std::ifstream file(file_path, std::ios::binary);
    if (file.is_open()) 
    {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if(content.find(signature) != std::string::npos)
        {
            std::unique_lock<std::mutex>(sg_cout_mutex);
            std::cout << "Infected file found: " << file_path << std::endl;
        }
    }
}

} // namespace cripty_project