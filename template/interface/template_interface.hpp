#pragma once

#define NO_NAME_MANGLING extern "C"

#include <list>
#include <tuple>
#include <sstream> // for std::istringstream
/* #include "../../../code/boost/include/boost/variant.hpp"
#include "../../../code/util-generic/split.hpp"
#include "../../../code/we/type/value.hpp" */
#include <stdio.h>
#include <flint/fmpz_mpoly.h>
#include <stdlib.h> // for malloc and free
#include <flint/fmpz.h>
#include <fstream>
#include <sys/resource.h>
#include <iostream>
#include <chrono>
#include <functional> // Include for std::function
#include <vector>     // Include for std::vector
#include <numeric>
#include <unordered_set> // for std::unordered_set
/*
using pnet_value = pnet::type::value::value_type;
using pnet_list = std::list<pnet_value>;
using pnet_list2d = std::list<std::list<pnet_value>>; */
using vector2d = std::vector<std::vector<int>>;
using list_type = std::list<std::string>; // Define list_type as std::list<std::string>


#include <iostream>
#include <stdexcept>
#include <vector>
#include <flint/fmpq_mpoly.h>
#include <flint/fmpq_mat.h>
#include <flint/fmpq.h>
#include <flint/fmpq_poly.h>
#include <flint/arith.h>
#include <flint/fmpz.h>
#include <flint/flint.h>
#include <string>
#include <config.hpp>
using vector2d = std::vector<std::vector<int>>;

#define RESOLVE_INTERFACE_FUNCTION(function) \
      (fhg::util::scoped_dlhandle \
      (config::implementation(), \
      RTLD_GLOBAL | RTLD_NOW | RTLD_DEEPBIND) \
      .sym<decltype(function)> \
      (BOOST_PP_STRINGIZE(function)))


NO_NAME_MANGLING

std::string singular_getReducedIBPSystem_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING

std::string singular_updateWeb_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING

std::string singular_reducedIBPs_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_MI_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);

NO_NAME_MANGLING
std::string singular_computeManyIBP_gpi(std::string const& res
      , std::string const& res1
      , int const& i
      , int const& k
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_targetInts_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_targetInt_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_getRedIBPs_gpi(std::string const& res
      , int const& j
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_IBP_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);

NO_NAME_MANGLING
std::string singular_alreadyIntheList_gpi(std::string const& res
      , std::string const& res1
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
int singular_size_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_over_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_seed_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING

std::string singular_equal_gpi(std::string const& res
      , std::string const& res1
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_setIBP_gpi(int res
      , std::string const& needed_library
      , std::string const& base_filename);