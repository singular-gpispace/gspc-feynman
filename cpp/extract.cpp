#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

void parseSSIFile(const std::string& filePath) {
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> extractedData;

    while (std::getline(inputFile, line)) {
        // Check if line contains '{'
        size_t start = line.find("{");
        size_t end = line.find("}", start);

        while (start != std::string::npos && end != std::string::npos) {
            // Extract the substring between the braces
            std::string data = line.substr(start, end - start + 1);
            extractedData.push_back(data);

            // Look for the next set of braces
            start = line.find("{", end);
            end = line.find("}", start);
        }
    }

    inputFile.close();

    // Print the extracted data in the desired format
    std::cout << "Extracted data: " << std::endl;

    // Print all blocks on the same line
    for (size_t i = 0; i < extractedData.size(); ++i) {
        std::cout << extractedData[i];
        if (i != extractedData.size() - 1) {
            std::cout << " ";  // Add space between blocks
        }
    }
    std::cout << std::endl; // Ensure a newline at the end
}

int main() {
    std::string directoryPath = "/home/atraore/gpi/try_gpi/feynman_ibp/example_dir/temp/";

    // Iterate through files in the directory
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::string fileName = entry.path().filename().string();

            // Check if filename starts with "waas"
            if (fileName.rfind("waas", 0) == 0) {
                std::cout << "Processing file: " << entry.path() << std::endl;
                parseSSIFile(entry.path().string());
            }
        }
    }

    return 0;
}
