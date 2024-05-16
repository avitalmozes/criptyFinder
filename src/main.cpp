
#include <iostream>
#include <fstream>
#include <filesystem>

#include "cripty.hpp"

namespace fs = std::filesystem;

int main(int argc, const char** argv) 
{ 
    if (argc < 3) 
    {
        std::cerr << "Usage: " << argv[0] << " <root_directory> <signature_file>" << std::endl;
        return 1;
    }

    // Check if the root directory exists
    std::string root_dir = argv[1];
    if (!fs::is_directory(root_dir)) 
    {
        std::cerr << "Error: Root directory does not exist or is not a directory." << std::endl;
        return 1;
    }

    // Check if the signature file exists
    std::string signature_file_path = argv[2];
    if (!fs::is_regular_file(signature_file_path)) 
    {
        std::cerr << "Error: Signature file does not exist or is not a regular file." << std::endl;
        return 1;
    }
    
    std::ifstream signature_file(signature_file_path);
    if (!signature_file.is_open()) 
    {
        std::cerr << "Error: Unable to open signature file." << std::endl;
        return 1;
    }

    std::string signature;
    signature.assign((std::istreambuf_iterator<char>(signature_file)), (std::istreambuf_iterator<char>()));
    signature_file.close();

    cripty_project::CriptyInfectedFilesFinder cripty_obj;
    cripty_obj.searchInfectedFiles(root_dir, signature);
    
    return 0; 
}
