#include "cripty.hpp"
#include "thread_pool.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace cripty_project
{

CriptyInfectedFilesFinder::CriptyInfectedFilesFinder()
{}

CriptyInfectedFilesFinder::~CriptyInfectedFilesFinder()
{}

void CriptyInfectedFilesFinder::searchInfectedFiles(const std::string &root_dir, const std::string &signature)
{
    for (const auto& entry : fs::recursive_directory_iterator(root_dir)) 
    {
        std::string file_path = entry.path().string();

        if(!fs::is_directory(file_path))
        {
            if(isInfected(file_path, signature))
            {
                std::cout << "Infected file found: " << file_path << std::endl;
            }
        }
    }
}

bool CriptyInfectedFilesFinder::isInfected(const std::string &file_path, const std::string &signature)
{
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) 
    {
        return false;
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content.find(signature) != std::string::npos;
}

} // namespace cripty_project