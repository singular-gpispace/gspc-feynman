#include <string>
#include <vector>
#include <Singular/libsingular.h>
#include "config.hpp"
#include "singular_functions.hpp"
#include <iostream>
#include <stdexcept>

// Add cleanup function for proper library unloading
extern "C" {
  void __attribute__((destructor)) cleanup_singular() {
    // Clean up any resources when library is unloaded
  }
}

std::string singular_mergeMI(std::string const& res,
                            std::string const& res1,
                            std::string const& needed_library,
                            std::string const& base_filename) {
    try {
        // Initialize Singular
        init_singular(config::singularLibrary().string());
        load_singular_library(needed_library);

        // Get worker ID
        std::string ids = worker();
        std::cout << "Worker " << ids << " starting merge operation" << std::endl;

        // Deserialize input tokens with validation
        std::cout << "Deserializing first input from " << res << std::endl;
        std::pair<int, lists> Res = deserialize(res, ids);
        if (!Res.second || Res.first != 1) {
            throw std::runtime_error("Invalid first input token");
        }
        
        std::cout << "Deserializing second input from " << res1 << std::endl;
        std::pair<int, lists> Res1 = deserialize(res1, ids);
        if (!Res1.second || Res1.first != 1) {
            throw std::runtime_error("Invalid second input token");
        }

        // Create arguments for Singular procedure
        std::cout << "Creating argument list for mergeMI" << std::endl;
        ScopedLeftv args(Res.first, lCopy(Res.second));
        ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

        // Call Singular procedure
        std::string function_name = "mergeMI";
        std::cout << "Calling " << function_name << " procedure" << std::endl;
        std::pair<int, lists> out = call_user_proc(function_name, needed_library, args);

        // Validate output
        if (!out.second || out.first != 1) {
            throw std::runtime_error("Invalid output from mergeMI procedure");
        }

        // Serialize result
        std::cout << "Serializing merged result" << std::endl;
        std::string out_filename = serialize(out.second, base_filename);
        std::cout << "Merge operation completed, result in " << out_filename << std::endl;

        return out_filename;
    } catch (const std::exception& e) {
        std::cerr << "Error in singular_mergeMI: " << e.what() << std::endl;
        throw;
    }
}

lists create_merge_token_input(const std::vector<std::vector<int>>& data_values) {
    try {
        // Allocate and initialize input list
        lists input = (lists)omAlloc0(sizeof(slists));
        if (!input) {
            throw std::runtime_error("Failed to allocate input list");
        }
        input->Init(2);

        // Create fieldnames list with "MI"
        lists fieldnames = (lists)omAlloc0(sizeof(slists));
        if (!fieldnames) {
            omFreeBin(input, slist_bin);
            throw std::runtime_error("Failed to allocate fieldnames list");
        }
        fieldnames->Init(1);
        fieldnames->m[0].rtyp = STRING_CMD;
        fieldnames->m[0].data = omStrDup("MI");
        input->m[0].rtyp = LIST_CMD;
        input->m[0].data = fieldnames;

        // Create data list with the vectors
        lists data = (lists)omAlloc0(sizeof(slists));
        if (!data) {
            omFreeBin(fieldnames, slist_bin);
            omFreeBin(input, slist_bin);
            throw std::runtime_error("Failed to allocate data list");
        }
        data->Init(1);

        // Create the list of vectors
        lists vectors = (lists)omAlloc0(sizeof(slists));
        if (!vectors) {
            omFreeBin(data, slist_bin);
            omFreeBin(fieldnames, slist_bin);
            omFreeBin(input, slist_bin);
            throw std::runtime_error("Failed to allocate vectors list");
        }
        vectors->Init(data_values.size());

        // Fill vectors with data
        for (size_t i = 0; i < data_values.size(); i++) {
            lists sublist = (lists)omAlloc0(sizeof(slists));
            if (!sublist) {
                // Cleanup already allocated memory
                for (size_t j = 0; j < i; j++) {
                    if (vectors->m[j].data) {
                        omFreeBin((lists)vectors->m[j].data, slist_bin);
                    }
                }
                omFreeBin(vectors, slist_bin);
                omFreeBin(data, slist_bin);
                omFreeBin(fieldnames, slist_bin);
                omFreeBin(input, slist_bin);
                throw std::runtime_error("Failed to allocate sublist");
            }
            sublist->Init(data_values[i].size());
            for (size_t j = 0; j < data_values[i].size(); j++) {
                sublist->m[j].rtyp = INT_CMD;
                sublist->m[j].data = (void*)(long)data_values[i][j];
            }
            vectors->m[i].rtyp = LIST_CMD;
            vectors->m[i].data = sublist;
        }

        // Connect all lists
        data->m[0].rtyp = LIST_CMD;
        data->m[0].data = vectors;
        input->m[1].rtyp = LIST_CMD;
        input->m[1].data = data;

        return input;
    } catch (const std::exception& e) {
        std::cerr << "Error in create_merge_token_input: " << e.what() << std::endl;
        throw;
    }
}

int main() {
    try {
        std::cout << "Starting Singular mergeMI test..." << std::endl;

        // Create first input token: MI_prev with data [[1,2], [3,4]]
        std::vector<std::vector<int>> data1 = {{1, 2}, {3, 4}};
        lists input_list1 = create_merge_token_input(data1);
        if (!input_list1) {
            throw std::runtime_error("Failed to create first input token");
        }
        std::cout << "Created first input token" << std::endl;

        // Create second input token: MI_new with data [[3,4], [5,6]]
        std::vector<std::vector<int>> data2 = {{3, 4}, {5, 6}};
        lists input_list2 = create_merge_token_input(data2);
        if (!input_list2) {
            omFreeBin(input_list1, slist_bin);
            throw std::runtime_error("Failed to create second input token");
        }
        std::cout << "Created second input token" << std::endl;

        // Serialize inputs
        std::string input_file1 = serialize(input_list1, "input_token1_");
        std::cout << "First input serialized to: " << input_file1 << std::endl;
        std::string input_file2 = serialize(input_list2, "input_token2_");
        std::cout << "Second input serialized to: " << input_file2 << std::endl;

        // Process tokens with mergeMI
        std::string library_path = config::installation.string() + "/lib/singular/example.lib";
        std::string result_file = singular_mergeMI(input_file1, input_file2, library_path, "output_merged_");
        std::cout << "Merge processing completed, result in: " << result_file << std::endl;

        // Read and verify result
        si_link link = ssi_open_for_read(result_file);
        if (!link) {
            throw std::runtime_error("Failed to open result file");
        }

        lists output_list = ssi_read_newstruct(link, "token");
        if (!output_list) {
            ssi_close_and_remove(link);
            throw std::runtime_error("Failed to read result token");
        }

        // Print result
        std::cout << "Merged token contents:" << std::endl;
        for (int i = 0; i <= output_list->nr; i++) {
            if (output_list->m[i].rtyp == LIST_CMD && output_list->m[i].data) {
                lists sublist = (lists)output_list->m[i].data;
                std::cout << "  List " << i << " (size " << sublist->nr + 1 << "):" << std::endl;
                if (i == 0) { // fieldnames
                    for (int j = 0; j <= sublist->nr; j++) {
                        if (sublist->m[j].rtyp == STRING_CMD) {
                            std::cout << "    " << (char*)sublist->m[j].data << std::endl;
                        }
                    }
                } else if (i == 1) { // data
                    lists data_list = (lists)sublist->m[0].data;
                    for (int j = 0; j <= data_list->nr; j++) {
                        if (data_list->m[j].rtyp == LIST_CMD && data_list->m[j].data) {
                            lists inner_list = (lists)data_list->m[j].data;
                            std::cout << "    Sublist " << j << ": ";
                            for (int k = 0; k <= inner_list->nr; k++) {
                                if (inner_list->m[k].rtyp == INT_CMD) {
                                    std::cout << (long)inner_list->m[k].data << " ";
                                }
                            }
                            std::cout << std::endl;
                        }
                    }
                }
            }
        }

        // Cleanup
        ssi_close_and_remove(link);
        omFreeBin(output_list, slist_bin);
        omFreeBin(input_list1, slist_bin);
        omFreeBin(input_list2, slist_bin);

        std::cout << "Test completed successfully" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}