#include <iostream>
#include <filesystem>
#include <cstdlib>  // for std::system
#include <TFile.h>
#include <TTree.h>

namespace fs = std::filesystem;

// Function to check if the "test" branch has any content in the ROOT file
bool checkTestBranch(const std::string& filePath) {
    TFile file(filePath.c_str(), "READ");

    // Check if the file was opened successfully
    if (file.IsZombie()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return false;
    }

    // Retrieve the TTree (assuming it's called "tree", adjust if necessary)
    TTree* tree = (TTree*)file.Get("tree");
    if (!tree) {
        std::cerr << "Error: TTree 'tree' not found in " << filePath << std::endl;
        return false;
    }

    // Check if the branch "test" exists in the TTree
    TBranch* testBranch = tree->GetBranch("test");
    if (!testBranch) {
        std::cerr << "'test' branch not found in " << filePath << std::endl;
        return false;
    }

    // Check if the 'test' branch has any entries
    if (testBranch->GetEntries() > 0) {
        return true;
    }

    return false;
}

// Function to iterate through files in the directory and process each ROOT file
void processFilesInDirectory(const std::string& directoryPath) {
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file()) {
            std::string filePath = entry.path().string();

            // Process only ROOT files
            if (filePath.find(".root") != std::string::npos) {
                std::cout << "Processing file: " << filePath << std::endl;

                // Check if the "test" branch has content
                if (checkTestBranch(filePath)) {
                    std::cout << "Non-empty 'test' branch found in " << filePath << ". Calling _saveimg.cpp..." << std::endl;

                    // Call the external script (_saveimg.cpp) here
                    std::string command = "./_saveimg " + filePath;
                    std::system(command.c_str());
                } else {
                    std::cout << "No content in 'test' branch for " << filePath << ". Skipping." << std::endl;
                }
            }
        }
    }
}

int main() {
    std::string directoryPath = "../../../Trinity/Data/20240718/RawDataMerged"; // Replace with your directory path
    processFilesInDirectory(directoryPath);
    return 0;
}
