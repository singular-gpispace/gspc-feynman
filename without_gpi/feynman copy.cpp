#include "singular_functions.hpp"
#include <Singular/libsingular.h>
#include "config.hpp"
#include <iostream>
#include <stdexcept>

// Function to call the simple Singular procedure
std::string call_simple_proc(lists input_list, const std::string& base_filename) {
    try {
        // Initialize Singular
        std::string singular_path = config::singularLibrary().string();
        init_singular(singular_path);
        std::cout << "Initialized Singular with path: " << singular_path << std::endl;

        // Load the library
        std::string install_prefix = config::installation.string();
        std::string library_path = install_prefix + "/lib/singular/example.lib";
        load_singular_library(library_path);
        std::cout << "Loaded library: " << library_path << std::endl;

        // Prepare input argument
        ScopedLeftv args(LIST_CMD, lCopy(input_list));
        std::cout << "Prepared input argument" << std::endl;

        // Call the Singular procedure
        auto [result_type, result_list] = call_user_proc("doubleList", library_path, args);
        if (result_type != LIST_CMD) {
            throw std::runtime_error("Unexpected return type from doubleList, got " + std::to_string(result_type));
        }
        std::cout << "Executed procedure: doubleList" << std::endl;

        // Print the result for debugging
        std::cout << "Result list size: " << result_list->nr + 1 << std::endl;
        for (int i = 0; i <= result_list->nr; i++) {
            std::cout << "Element " << i << ": type " << result_list->m[i].rtyp 
                      << ", value " << (long)result_list->m[i].data << std::endl;
        }

        // Serialize the result
        std::string output_file = serialize(result_list, base_filename);
        std::cout << "Serialized result to: " << output_file << std::endl;

        return output_file;

    } catch (const std::exception& e) {
        std::cerr << "Error in call_simple_proc: " << e.what() << std::endl;
        throw;
    }
}

// Create a simple input list
lists create_simple_input() {
    lists input = (lists)omAlloc0(sizeof(slists));
    input->Init(3); // List of 3 integers
    for (int i = 0; i < 3; i++) {
        input->m[i].rtyp = INT_CMD;
        input->m[i].data = (void*)(long)(i + 1); // [1, 2, 3]
    }
    return input;
}

int main() {
    try {
        std::cout << "Starting simple procedure test" << std::endl;

        // Create input list
        lists input_list = create_simple_input();
        std::cout << "Created input list" << std::endl;

        // Call the simple procedure
        std::string result_file = call_simple_proc(input_list, "test_output");
        std::cout << "Computed result, output file: " << result_file << std::endl;

        // Read and verify results
        si_link link = ssi_open_for_read(result_file);
        lists output_list = ssi_read_newstruct(link, "list"); // Expect a simple list
        std::cout << "Read result from file. Size: " << output_list->nr + 1 << std::endl;
        for (int i = 0; i <= output_list->nr; i++) {
            std::cout << "Output element " << i << ": " << (long)output_list->m[i].data << std::endl;
        }
        ssi_close_and_remove(link);

        // Cleanup
        omFreeBin(output_list, slist_bin);
        omFreeBin(input_list, slist_bin);
        std::cout << "Cleaned up memory" << std::endl;

        std::cout << "Test completed successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error in main: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}