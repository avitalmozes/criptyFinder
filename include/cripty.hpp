#ifndef CRIPTY_INFECTED_FILES_FINDER_HPP
#define CRIPTY_INFECTED_FILES_FINDER_HPP

#include <string>
#include <memory>

namespace cripty_project
{

class ThreadPool;

class CriptyInfectedFilesFinder
{
public:
    CriptyInfectedFilesFinder();
    ~CriptyInfectedFilesFinder();
    void searchInfectedFiles(const std::string& root_dir, const std::string& signature);

private:
    bool isInfected(const std::string& file_path, const std::string& signature);

    std::unique_ptr<ThreadPool>thread_pool;

};

} // namespace cripty_project

#endif //CRIPTY_INFECTED_FILES_FINDER_HPP
