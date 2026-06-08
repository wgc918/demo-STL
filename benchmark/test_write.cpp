#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

int main()
{
    std::filesystem::path results_dir = "results";
    if (!std::filesystem::exists(results_dir))
    {
        std::filesystem::create_directories(results_dir);
    }

    std::string filepath = (results_dir / "test.csv").string();
    std::cout << "Writing to: " << std::filesystem::absolute(filepath) << std::endl;

    std::ofstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "Failed to open file!" << std::endl;
        return 1;
    }

    file << "Hello,World\n";
    file << "1,2\n";
    file.close();
    std::cout << "File written successfully." << std::endl;
    return 0;
}