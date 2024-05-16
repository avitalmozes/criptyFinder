#ifndef CRIPTY_INFECTED_FILES_FINDER_HPP
#define CRIPTY_INFECTED_FILES_FINDER_HPP

#include <string>

namespace cripty
{

class CriptyInfectedFilesFinder
{
public:
    CriptyInfectedFilesFinder();
    ~CriptyInfectedFilesFinder();
    void searchInfectedFiles(const std::string& root_dir, const std::string& signature);

private:
    bool isInfected(const std::string& file_path, const std::string& signature);

};

}

#endif //CRIPTY_INFECTED_FILES_FINDER_HPP
