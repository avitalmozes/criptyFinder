
#include <iostream>
#include <fstream>
#include <filesystem>
#include <mutex>

#include "crypty_finder.hpp"

namespace fs = std::filesystem;

bool ProgramArgsValid(int argc, const char** argv);
std::string readSignature(const std::string &signature_file_path);

int main(int argc, const char** argv) 
{ 
    if(ProgramArgsValid(argc,argv))
    {
        std::string search_root_dir = argv[1];
        std::string signature_file_path = argv[2];

        std::string signature = readSignature(signature_file_path);

        if(!signature.empty())
        {
            crypty_project::CryptyInfectedFilesFinder crypty_finder;
            crypty_finder.searchInfectedFiles(search_root_dir, signature);
            return 0; 
        }
    }

    return 1;
}

bool ProgramArgsValid(int argc, const char** argv)
{
    if (argc < 3) 
    {
        std::cerr << "Usage: " << argv[0] << " <root_directory> <signature_file>" << std::endl;
        return false;
    }

    // Check if the root directory exists
    std::string root_dir = argv[1];
    if (!fs::is_directory(root_dir)) 
    {
        std::cerr << "Error: first argument - root directory does not exist or is not a directory." << std::endl;
        return false;
    }

    // Check if the signature file exists
    std::string signature_file_path = argv[2];
    if (!fs::exists(signature_file_path)) 
    {
        std::cerr << "Error: second argument - Signature file does not exist" << std::endl;
        return false;
    }

    return true;
}

std::string readSignature(const std::string &signature_file_path)
{
    std::string signature;
    std::ifstream signature_file(signature_file_path);

    if (signature_file.is_open()) 
    {
        signature.assign((std::istreambuf_iterator<char>(signature_file)), (std::istreambuf_iterator<char>()));
        signature_file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open signature file." << std::endl;
    }

    return signature;   
}