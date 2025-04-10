#pragma once

#include <string>
#include <Singular/libsingular.h>
#include "singular_functions.hpp"

// Function declarations
std::string singular_double_token(std::string const& input_filename, 
                                std::string const& needed_library,
                                std::string const& base_filename);

std::string singular_mergeMI_gpi(std::string const& res,
                                std::string const& res1,
                                std::string const& needed_library,
                                std::string const& base_filename);

lists create_token_input(); 