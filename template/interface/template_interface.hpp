#pragma once

#define NO_NAME_MANGLING extern "C"
#include <config.hpp>
#include <boost/variant.hpp>

#include <we/type/literal/control.hpp>
#include <we/type/bitsetofint.hpp>
#include <we/type/bytearray.hpp>
#include <we/type/value.hpp>
#include <map>
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
int singular_alreadyIntheList_gpi(std::string const& res
      , std::string const& res1
      , std::string const& needed_library
   );
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
NO_NAME_MANGLING
std::string singular_indpndIBP_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_masterAndTailIntgrals_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string singular_Return_place(std::string const& res
      , int const& j
      , std::string const& needed_library
      , std::string const& base_filename);

NO_NAME_MANGLING
std::string singular_updateWeb_later(std::string const& res
      , std::string const& res1
      , int const& j
      , int const& k
      , std::string const& needed_library
      , std::string const& base_filename);
NO_NAME_MANGLING
std::string vector2dToString(const std::vector<std::vector<int>>& vec);
NO_NAME_MANGLING
std::vector<std::vector<int>> stringToVector2d(const std::string& str);
NO_NAME_MANGLING
std::vector<int> getFirstList(std::vector<std::vector<int>>& vec2d);
NO_NAME_MANGLING
std::pair<int, int> string_to_pair(const std::string& pair_str) ;
NO_NAME_MANGLING
std::string singular_replace_two(std::string const& res
    , int const& j
    , int const& k
    , std::string const& needed_library
    , std::string const& base_filename);
NO_NAME_MANGLING
    std::string singular_computeManyIBPjk_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename);
NO_NAME_MANGLING
    int singular_updateWeb_sizejk(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename);
NO_NAME_MANGLING
    std::string singular_computeM1_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename);
    NO_NAME_MANGLING
     
std::string singular_computeM2_gpi(std::string const& res
    , std::string const& res1
    , int const& j
    , int const& k
    , std::string const& needed_library
    , std::string const& base_filename);

    NO_NAME_MANGLING
std::string singular_intersection_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename);
    NO_NAME_MANGLING


std::string singular_std_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename);
      NO_NAME_MANGLING

std::string singular_assign_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename
);
NO_NAME_MANGLING
std::string printGpiTokenContent(const std::string & tokenString, const std::string & needed_library);
NO_NAME_MANGLING
std::string create_feynman_data_list(
    const std::vector<std::string>& feynman_inputs,  // Vector of serialized feynman structs
    const std::string& library_name,                 // Singular library name
    const std::string& base_filename                 // Base filename for serialization
);
NO_NAME_MANGLING
std::string singular_mergeFeynman_gpi(std::string const& res
      , std::string const& needed_library
      , std::string const& base_filename);

NO_NAME_MANGLING
std::string singular_deserialize_feynman_token(
      const std::string& token_str,
      const std::string& library_name,
      const std::string& base_filename
  );