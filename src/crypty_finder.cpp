#include "crypty_finder.hpp"
#include "thread_pool.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>

namespace fs = std::filesystem;

namespace crypty_project
{
 // can be modified due to computer abilities
static constexpr size_t NUM_THREADS_MULTI_FILES = 10;
static constexpr uint64_t CHUNK_SPLIT = 10000000;

CryptyInfectedFilesFinder::CryptyInfectedFilesFinder()
{
    m_thread_pool_multi_files = std::make_unique<ThreadPool>(NUM_THREADS_MULTI_FILES);
}

CryptyInfectedFilesFinder::~CryptyInfectedFilesFinder()
{
}

void CryptyInfectedFilesFinder::searchInfectedFiles(const std::string &root_dir, const std::string &signature)
{
    for (const auto& entry : fs::recursive_directory_iterator(root_dir)) 
    {
        std::string file_path = entry.path().string();

        if(fs::is_regular_file(file_path))
        {
            // in order to execute member function in a threadpool, the function need to be
            // a lamda function and its arguments need to be captured(including "this") 
            std::function<void()> captured_lamda_scanFile_func = [this, file_path, signature]() 
            { 
                this->scanFile(file_path, signature); 
            };
            
            m_thread_pool_multi_files->enqueue(captured_lamda_scanFile_func);
        }
    }
}

bool CryptyInfectedFilesFinder::smallFileInfected(std::ifstream &file, const std::string &signature)
{
    if (file.is_open()) 
    {        
        try
        {
            // scan all file into a string, return signature seraching result
            std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
            return(content.find(signature) != std::string::npos);
        }
        catch(const std::ios_base::failure& e)        
        {
            std::cerr << e.what() << std::endl;
        }
    }

    return false;
}

bool CryptyInfectedFilesFinder::bigFileInfected(std::ifstream &file, const std::string &signature, size_t file_size)
{
    if(file.is_open())
    {
        // Calculate the number of chunks of file to be scaned 
        size_t num_chunks = static_cast<size_t>(std::ceil(static_cast<double>(file_size) / CHUNK_SPLIT));

        for (size_t i = 0; i < num_chunks; ++i) 
        {
            std::streampos start_pos = i * CHUNK_SPLIT;

            // when calculating the end position, size of the signature is added to make sure
            // the cutting points of chuncks are also scanned with the signature (-1 to not
            // add the '\0' char to the size) 
            std::streampos end_pos = std::min((i + 1) * CHUNK_SPLIT + signature.size() - 1, file_size);

            // TODO: paralellize scaning the chunks
            // if one part found infected, no need to continue the scan.
            if(scanChunk(start_pos, end_pos, signature, file))
            {
                return true;
            }
        }
    }

    return false;
}

bool CryptyInfectedFilesFinder::scanChunk(std::streampos start_pos, std::streampos end_pos,
                                        const std::string& signature, std::ifstream &file)
{
    if (file.is_open()) 
    {        
        try
        {
            // Seek to the start position of the chunk
            file.seekg(start_pos);

            // Read the chunk content
            std::vector<char> chunk(end_pos - start_pos);
            file.read(chunk.data(), end_pos - start_pos);

            // scan all chunk into a string, return signature seraching result
            std::string chunk_content(chunk.begin(), chunk.end());
            return(chunk_content.find(signature) != std::string::npos);
        }
        catch(const std::ios_base::failure& e)
        {
            std::cerr << e.what() << std::endl;
        } 
    }

    return false;
}

void CryptyInfectedFilesFinder::scanFile(const std::string &file_path, const std::string &signature)
{
    std::ifstream file(file_path, std::ios::binary);
    if (file.is_open()) 
    {
        try
        {
            // Get the file size
            file.seekg(0, std::ios::end);
            size_t file_size = static_cast<size_t>(file.tellg());
            file.seekg(0, std::ios::beg);

            // check if file is big need to be splited
            if(file_size > CHUNK_SPLIT) 
            {
                if(bigFileInfected(file, signature, file_size))
                {
                    std::unique_lock<std::mutex> lock(m_cout_mutex);
                    std::cout << file_path << " is infected!" << std::endl;
                }
            }
            else
            {
                if(smallFileInfected(file, signature))
                {
                    std::unique_lock<std::mutex> lock(m_cout_mutex);
                    std::cout << file_path << " is infected!" << std::endl;
                }                
            }

            file.close();
        }
        catch(const std::ios_base::failure& e)
        {
            std::cerr << e.what() << std::endl;
        }   
    }
    else
    {
        std::cerr << "problem opening " << file_path << std::endl;
    }    
}

} // namespace crypty_project