#ifndef CRIPTY_INFECTED_FILES_FINDER_HPP
#define CRIPTY_INFECTED_FILES_FINDER_HPP

#include <string>
#include <memory>
#include <mutex>

namespace cripty_project
{

class ThreadPool;

class CriptyInfectedFilesFinder
{
public:
    CriptyInfectedFilesFinder();
    ~CriptyInfectedFilesFinder();
    void searchInfectedFiles(const std::string& root_dir, const std::string& signature);
    void scanFile(const std::string& filePath, const std::string& signature); 
    bool scanChunk(std::streampos startPos, std::streampos endPos,
                                        std::string signature, std::string file_path); 



private:
    void scanSmallFile(const std::string& file_path, const std::string& signature);

    std::unique_ptr<ThreadPool>m_thread_pool_multi_files;
    std::unique_ptr<ThreadPool>m_thread_pool_big_file;
    std::mutex m_cout_mutex;
    std::mutex m_big_file_mutex;
};

} // namespace cripty_project

#endif //CRIPTY_INFECTED_FILES_FINDER_HPP
