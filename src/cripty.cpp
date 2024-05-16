#include "cripty.hpp"
#include "thread_pool.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <algorithm>

namespace fs = std::filesystem;

namespace cripty_project
{

static constexpr size_t NUM_THREADS_MULTI_FILES = 10;
static constexpr size_t NUM_THREADS_BIG_FILE = 20;

static constexpr uint64_t CHUNK_SPLIT = 1000000;

CriptyInfectedFilesFinder::CriptyInfectedFilesFinder()
{
    m_thread_pool_multi_files = std::make_unique<ThreadPool>(NUM_THREADS_MULTI_FILES);
    m_thread_pool_big_file = std::make_unique<ThreadPool>(NUM_THREADS_BIG_FILE);
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
            std::function<void()> captured_lamda_scanFile_func = [this, file_path, signature]() 
            { 
                this->scanFile(file_path, signature); 
            };
            
            m_thread_pool_multi_files->enqueue(captured_lamda_scanFile_func);
        }
    }
}

void CriptyInfectedFilesFinder::scanSmallFile(const std::string &file_path, const std::string &signature)
{
    std::ifstream file(file_path, std::ios::binary);
    if (file.is_open()) 
    {        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        if(content.find(signature) != std::string::npos)
        {
            std::unique_lock<std::mutex> lock(m_cout_mutex);
            std::cout << "Infected file found: " << file_path << std::endl;
        }
    }
    else
    {
        std::cerr << "problem opening " << file_path << std::endl;
    }
}

bool CriptyInfectedFilesFinder::scanChunk(std::streampos start_pos, std::streampos end_pos,
                                        std::string signature, std::string file_path)
{
    std::ifstream file(file_path, std::ios::binary);
    if (file.is_open()) 
    {        
        // Seek to the start position of the chunk
        file.seekg(start_pos);

        // Read the chunk content
        std::vector<char> chunk(end_pos - start_pos);
        file.read(chunk.data(), end_pos - start_pos);

        // Convert the chunk content to string
        std::string chunk_content(chunk.begin(), chunk.end());

        // Search for the signature in the chunk
        if(chunk_content.find(signature) != std::string::npos)
        {
            std::unique_lock<std::mutex> lock(m_cout_mutex);
            std::cout << "Infected file found: " << file_path << std::endl;
            return true;
        }    
    }
    else
    {
        std::cerr << "problem opening " << file_path << std::endl;
    }

    return false;
}

void CriptyInfectedFilesFinder::scanFile(const std::string &file_path, const std::string &signature)
{
    std::ifstream file(file_path, std::ios::binary);
    if (file.is_open()) 
    {
        // Get the file size
        file.seekg(0, std::ios::end);
        size_t file_size = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        if(file_size > CHUNK_SPLIT) // it is a big file
        {
            // Calculate the number of chunks
            size_t num_chunks = static_cast<size_t>(std::ceil(static_cast<double>(file_size) / CHUNK_SPLIT));

            // Create a thread pool and futures for each chunk
            std::vector<std::future<bool>> futures;
            for (size_t i = 0; i < num_chunks; ++i) 
            {
                std::streampos start_pos = i * CHUNK_SPLIT;
                std::streampos end_pos = std::min((i + 1) * CHUNK_SPLIT + signature.size() - 1, file_size);

                std::function<bool()> captured_lamda_func = [this, start_pos, end_pos, signature, file_path]() -> bool 
                {
                    return this->scanChunk(start_pos, end_pos, signature, file_path);
                };

                futures.push_back(m_thread_pool_big_file->enqueue(captured_lamda_func));
            }

            // Wait for all threads to finish and check the results
            for (auto& future : futures) 
            {
                if (future.get()) 
                {
                    return; // Signature found in one of the chunks, no need to continue scanning
                }
            }
        }
        else
        {
            scanSmallFile(file_path, signature);
        }

        file.close();
    }
    else
    {
        std::cerr << "problem opening " << file_path << std::endl;
    }    
}

} // namespace cripty_project