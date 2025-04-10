#include <iostream>
#include <cstdlib>
#include <string>
#include <filesystem>

int main() {
    std::cout << "Hello, this is a test program!" << std::endl;
    
    // Print environment variables
    const char* ld_library_path = std::getenv("LD_LIBRARY_PATH");
    std::cout << "LD_LIBRARY_PATH: " << (ld_library_path ? ld_library_path : "not set") << std::endl;
    
    // Print current working directory
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    
    // Check if Singular library exists
    std::string singular_lib = "/home/atraore/singular-gpispace/spack/opt/spack/linux-ubuntu22.04-skylake/gcc-11.3.0/singular-4.4.0p2-k7rgdkzo5prqsvxjckejvcdvxgjr64bk/lib/libSingular.so";
    if (std::filesystem::exists(singular_lib)) {
        std::cout << "Singular library found at: " << singular_lib << std::endl;
    } else {
        std::cout << "Warning: Singular library not found at: " << singular_lib << std::endl;
    }
    
    return 0;
} 