#ifndef CRYPTY_INFECTED_FILES_FINDER_HPP
#define CRYPTY_INFECTED_FILES_FINDER_HPP

#include <string> // std::string
#include <memory> // std::unique_ptr
#include <mutex>  // std::mutex

namespace crypty_project { // Namespace Declaration: crypty_project

class ThreadPool; // Forward Declaration: ThreadPool class

class CryptyInfectedFilesFinder
{
public:
    /**
     * @brief Default constructor 
     */
    CryptyInfectedFilesFinder();

    /**
     * @brief Destructor 
     */
    ~CryptyInfectedFilesFinder();

    /**
     * @brief Searches for infected files in the specified root directory.
     * 
     * @param root_dir The root directory to search for infected files.
     * @param signature The signature indicating infection.
     */
    void searchInfectedFiles(const std::string& root_dir, const std::string& signature);

    /**
     * @brief Scans a single file for infection.
     * 
     * @param file_path The path of the file to scan.
     * @param signature The signature indicating infection.
     */
    void scanFile(const std::string& file_path, const std::string& signature); 

    /**
     * @brief Scans a chunk of a file for infection.
     * 
     * @param start_pos The starting position of the chunk.
     * @param end_pos The ending position of the chunk.
     * @param signature The signature indicating infection.
     * @param file The file stream to scan.
     * @return True if the chunk is infected, false otherwise.
     */
    bool scanChunk(std::streampos start_pos, std::streampos end_pos,
                                        const std::string& signature, std::ifstream& file); 

private:
    /**
     * @brief Checks if a small file is infected.
     * 
     * @param file The file stream to check.
     * @param signature The signature indicating infection.
     * @return True if the file is infected, false otherwise.
     */
    bool smallFileInfected(std::ifstream& file, const std::string& signature);

    /**
     * @brief Checks if a big file is infected, splits the file to chunks of scaning
     * 
     * @param file The file stream to check.
     * @param signature The signature indicating infection.
     * @param file_size The size of the file.
     * @return True if the file is infected, false otherwise.
     */
    bool bigFileInfected(std::ifstream& file, const std::string& signature, size_t file_size);

    std::unique_ptr<ThreadPool> m_thread_pool_multi_files; // Unique pointer to thread pool for multi-file scanning
    std::mutex m_cout_mutex;                               // Mutex for thread-safe cout operations
};

} // namespace crypty_project

#endif //CRYPTY_INFECTED_FILES_FINDER_HPP
