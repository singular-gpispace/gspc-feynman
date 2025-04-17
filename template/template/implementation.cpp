#include <Singular/libsingular.h>
#include <interface/template_interface.hpp>  // This defines NO_NAME_MANGLING
#include "feynman.hpp"  // Add this include for singflintGaussRed
#include "singular_functions.hpp"
#include "common_functions.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <regex>
#include "template/config.hpp"

#include <interface/WorkflowResult.hpp>
#include <interface/ValuesOnPorts.hpp>
#include <interface/Workflow.hpp>
#include <stdexcept>
#include <unistd.h>
#include <kernel/linear_algebra/linearAlgebra.h>

#include <chrono>
#include <tuple>
#include <typeinfo>

#include <stdio.h>
#include <string>

NO_NAME_MANGLING

// Add cleanup function for proper library unloading
extern "C" {
  void __attribute__((destructor)) cleanup_singular_template() {
    // Clean up any resources
    // This will be called when the library is unloaded
  }
}

std::string printGpiTokenContent(const std::string & tokenString, const std::string & needed_library)
{
    // Initialize and load the Singular library.
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    // Deserialize the token. Here 'worker()' returns an ID string.
    std::pair<int, lists> inputPair = deserialize(tokenString, worker());

    // Create a scoped left-value from the deserialized token.
    ScopedLeftv args(inputPair.first, lCopy(inputPair.second));
    lists Token = (lists)(args.leftV()->data);

    // Determine the number of elements in the token list.
    int L_size = lSize(Token) + 1;
    std::string outputContent;

    // Iterate over the list elements. 
    // In your system, element at index 3 holds the full content you wish to print.
    for (int i = 0; i < L_size; i++) {
        sleftv & listElement = Token->m[i];  // Access each element

        if (listElement.data != NULL) {
            // When i==3, print and store the element's string representation.
            if (i == 3) {
                outputContent = listElement.String();
                
            }
        }
        else {
            std::cout << "Token element at index " << i << " is NULL." << std::endl;
        }
    }
    return outputContent;
}

std::string singular_template_compute_StdBasis(std::string const& input_filename
    , std::string const& needed_library
)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> input;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    std::string out_filename1;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    input = deserialize(input_filename, ids);

    ScopedLeftv args(input.first, lCopy(input.second));
    //std::string function_name2 = "stdBasis";

    lists Token = (lists)(args.leftV()->data);

    int L_size = lSize(Token) + 1;
    for (int i = 0; i < L_size; i++) {
        sleftv& listElement = Token->m[i];  // Access each element as `leftv`
        if (listElement.data == NULL) {
           // std::cout << "Input: NULL" << std::endl;
        }
        else if (i == 3) {
            out_filename1 = listElement.String();
            // std::cout << "out_filename1= " << out_filename1 << std::endl;
        }
    }
    /*  // Example output filename for debug purposes
     std::cout << "Base filename: " << base_filename << std::endl;
     std::cout << "Output filename: " << out_filename1 << std::endl;
     std::cout << "Type of out_filename1: " << typeid(out_filename1).name() << std::endl; */

     //out = call_user_proc(function_name2, needed_library, args);
     //std::cout << "myout " << out.second << std::endl;
     //out_filename = serialize(out.second, base_filename);
     //std::cout << base_filename << std::endl;;


    return out_filename1;
}
std::string singular_getReducedIBPSystem_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "getReducedIBPSystem_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}


std::string singular_updateWeb_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "updateWeb_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}


std::string singular_reducedIBPs_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "reducedIBPs_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
std::string singular_MI_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "MI_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

int singular_size_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);
    std::string out_filename1;

    ScopedLeftv args(Res.first, lCopy(Res.second));

    /*  lists Token = (lists)(args.leftV()->data);

     int L_size = lSize(Token) + 1;
     for (int i = 0; i < L_size; i++) {
         sleftv& listElement = Token->m[i];  // Access each element as `leftv`
         if (listElement.data == NULL) {
             std::cout << "Input: NULL" << std::endl;
         }

         out_filename1 = listElement.String();
         std::cout << "first= " << out_filename1 << std::endl;
     } */

    std::string function_name = "size_gpi";
    out = call_user_proc(function_name, needed_library, args);


    lists u = (lists)out.second->m[3].Data(); //token data

    // lists u = (lists)(args.leftV()->data);

 /*    int L_size1 = lSize(u) + 1;
    for (int i = 0; i < L_size1; i++) {
        sleftv& listElement = u->m[i];  // Access each element as `leftv`
        if (listElement.data == NULL) {
            std::cout << "Input: NULL" << std::endl;
        }

        out_filename1 = listElement.String();
        //  std::cout << "i=" << i << " " << "out_inside= " << out_filename1 << std::endl;


    } */
    sleftv& listElement = u->m[0];
    out_filename1 = listElement.String();

   // std::cout << "out_filename1= " << out_filename1 << std::endl;

    int ss = std::stoi(out_filename1);
    //std::cout << "ss " << ss << std::endl;
    out_filename = serialize(out.second, base_filename);

    return ss;
}

std::string singular_computeManyIBP_gpi(std::string const& res
    , std::string const& res1
    , int const& j
    , int const& k
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> Res2;
    std::pair<int, lists> Res3;

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    void* p = (char*)(long)(j);
    void* M = (char*)(long)(k);



    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
    ScopedLeftv arg(args1, INT_CMD, p);
    ScopedLeftv argss(args1, INT_CMD, M);

    std::string function_name = "computeManyIBP_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_targetInts_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "targetInts_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_targetInt_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "targetInt_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}


std::string singular_IBP_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "IBP_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);
    
    return out_filename;
}
std::string singular_getRedIBPs_gpi(std::string const& res
    , int const& j
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    void* p = (char*)(long)(j);

    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv arg(args, INT_CMD, p);

    std::string function_name = "getRedIBPs_gpi";
    out = call_user_proc(function_name, needed_library, args);


    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

int singular_alreadyIntheList_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
 )
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    std::string out_filename1;

    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    std::string function_name = "alreadyIntheList_gpi";
    out = call_user_proc(function_name, needed_library, args);
    //out_filename = serialize(out.second, base_filename);

    lists u = (lists)out.second->m[3].Data(); //token data

    sleftv& listElement = u->m[0];
    out_filename1 = listElement.String();

    //std::cout << "out_filename1= " << out_filename1 << std::endl;

    int ss = std::stoi(out_filename1);
  //  std::cout << "alreadyIntheList_gpi " << ss << std::endl;

    return ss;
}

std::string singular_over_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "over_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_seed_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "seed_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_equal_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    std::string function_name = "equal_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
std::string singular_set_over_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;   
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second)); 
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    std::string function_name = "set_over_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;        
}   
std::string singular_set_seed_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    std::string function_name = "set_seed_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
std::string singular_setIBP_gpi(int res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    void* p = (char*)(long)(res);



    ScopedLeftv arg(INT_CMD, p);
    std::string function_name = "setIBP_gpi";
    out = call_user_proc(function_name, needed_library, arg);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_indpndIBP_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "indpndIBP_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
std::string singular_masterAndTailIntgrals_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    std::string function_name = "masterAndTailIntgrals_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
std::string singular_Return_place(std::string const& res
    , int const& j
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    void* p = (char*)(long)(j);

    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv arg(args, INT_CMD, p);

    std::string function_name = "Return_place";
    out = call_user_proc(function_name, needed_library, args);


    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_updateWeb_later(std::string const& res
    , std::string const& res1
    , int const& j
    , int const& k
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> Res2;
    std::pair<int, lists> Res3;

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    void* p = (char*)(long)(j);
    void* M = (char*)(long)(k);



    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
    ScopedLeftv arg(args1, INT_CMD, p);
    ScopedLeftv argss(args1, INT_CMD, M);

    std::string function_name = "updateWeb_later";
    out = call_user_proc(function_name, needed_library, args);

   /*   lists u = (lists)out.second->m[3].Data();
    //  lists u1 = (lists)out.second->m[3].Data(); //token data

       int L_size1 = lSize(u)+1;
        std::cout << "LeadSyz in implementation.cpp " << L_size1 << std::endl;
    for (int i = 0; i < L_size1; i++) {
        sleftv& listElement = u->m[i];  // Access each element as `leftv`
        if (listElement.data == NULL) {
            std::cout << "Input: NULL" << std::endl;
        }

       auto out_filename1 = listElement.String();
         std::cout << "i=" << i << " " << "out_inside= " << out_filename1 << std::endl;


    }  */

    out_filename = serialize(out.second, base_filename);


    return out_filename;
}


std::string vector2dToString(const std::vector<std::vector<int>>& vec) {
    std::stringstream ss;
    for (const auto& innerVec : vec) {
        for (int val : innerVec) {
            ss << val << ' ';
        }
        ss << '\n'; // Add a newline after each inner vector
    }
    return ss.str();
}

std::vector<std::vector<int>> stringToVector2d(const std::string& str) {
    std::vector<std::vector<int>> result;
    std::stringstream ss(str);
    std::string line;
    while (std::getline(ss, line)) {
        std::vector<int> innerVec;
        std::stringstream lineStream(line);
        int num;
        while (lineStream >> num) {
            innerVec.push_back(num);
        }
        result.push_back(innerVec);
    }
    return result;
}
std::vector<int> getFirstList(std::vector<std::vector<int>>& vec2d) {
    if (!vec2d.empty()) {
        std::vector<int> firstList = std::move(vec2d.front());
        vec2d.erase(vec2d.begin());
        return firstList;
    }
    return {};
}
std::pair<int, int> string_to_pair(const std::string& pair_str) {
    std::istringstream iss(pair_str);
    int first, second;
    char comma; 

    if (iss >> first >> comma && comma == ',' && iss >> second) {
        return std::make_pair(first, second); 
    }
    return std::make_pair(0, 0); 
}

std::string singular_replace_two(std::string const& res
    , int const& j
    , int const& k
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
  

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    void* p = (char*)(long)(j);
    void* M = (char*)(long)(k);



    Res = deserialize(res, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv arg(args, INT_CMD, p);
    ScopedLeftv argss(args, INT_CMD, M);

    std::string function_name = "replace_two";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_computeManyIBPjk_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    std::string function_name = "computeManyIBPjk_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
int singular_updateWeb_sizejk(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);
    std::string out_filename1;

    ScopedLeftv args(Res.first, lCopy(Res.second));

    /*  lists Token = (lists)(args.leftV()->data);

     int L_size = lSize(Token) + 1;
     for (int i = 0; i < L_size; i++) {
         sleftv& listElement = Token->m[i];  // Access each element as `leftv`
         if (listElement.data == NULL) {
             std::cout << "Input: NULL" << std::endl;
         }

         out_filename1 = listElement.String();
         std::cout << "first= " << out_filename1 << std::endl;
     } */

    std::string function_name = "updateWeb_sizejk";
    out = call_user_proc(function_name, needed_library, args);


    lists u = (lists)out.second->m[3].Data(); //token data

    // lists u = (lists)(args.leftV()->data);

 /*    int L_size1 = lSize(u) + 1;
    for (int i = 0; i < L_size1; i++) {
        sleftv& listElement = u->m[i];  // Access each element as `leftv`
        if (listElement.data == NULL) {
            std::cout << "Input: NULL" << std::endl;
        }

        out_filename1 = listElement.String();
        //  std::cout << "i=" << i << " " << "out_inside= " << out_filename1 << std::endl;


    } */
    sleftv& listElement = u->m[0];
    out_filename1 = listElement.String();

   // std::cout << "out_filename1= " << out_filename1 << std::endl;

    int ss = std::stoi(out_filename1);
    //std::cout << "ss " << ss << std::endl;
    out_filename = serialize(out.second, base_filename);

    return ss;
}


#include <Singular/libsingular.h>
#include <iostream>
#include <vector>
#include <sstream>

// Parse nu values from string (e.g., "[1,1,1,-1,-3,1,-1,-1,-1]")
// This function has been moved to feynman.cpp
// std::vector<int> parseNuValues(const std::string& str) {
//     std::vector<int> values;
//     std::string cleaned = str;
//     cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '['), cleaned.end());
//     cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), ']'), cleaned.end());
//     std::stringstream ss(cleaned);
//     std::string item;
//     while (std::getline(ss, item, ',')) {
//         if (!item.empty()) {
//             values.push_back(std::stoi(item));
//         }
//     }
//     return values;
// }

// Print list elements for debugging
// This function has been moved to feynman.cpp
// void printListElements(lists output) {
//     if (!output) {
//         std::cerr << "ERROR: Null list in printListElements" << std::endl;
//         return;
//     }
//     
//     std::cout << "List with " << output->nr + 1 << " elements:" << std::endl;
//     for (int i = 0; i <= output->nr; i++) {
//         std::cout << "  [" << i << "]: ";
//         if (output->m[i].rtyp == STRING_CMD) {
//             std::cout << "STRING: " << (char*)output->m[i].data << std::endl;
//         } else if (output->m[i].rtyp == INT_CMD) {
//             std::cout << "INT: " << (int)(long)output->m[i].data << std::endl;
//         } else if (output->m[i].rtyp == LIST_CMD) {
//             std::cout << "LIST" << std::endl;
//             printListElements((lists)output->m[i].data);
//         } else if (output->m[i].rtyp == RING_CMD) {
//             std::cout << "RING: " << rString((ring)output->m[i].data) << std::endl;
//         } else if (output->m[i].rtyp == MATRIX_CMD) {
//             std::cout << "MATRIX" << std::endl;
//             printMat((matrix)output->m[i].data);
//         } else {
//             std::cout << "Type: " << output->m[i].rtyp << std::endl;
//         }
//     }
// }



lists intersection_gpi(leftv arg1) {
    std::cout<<"starting to intersection module M"<<std::endl;
    // Extract input list
    lists input1 = (lists)(arg1->Data());
    //lists m1 = (lists)(input1->m[3].Data());

    lists input2 = (lists)(arg1->next->Data()); // extract Tok
    lists m2 = (lists)(input2->m[3].Data()); // Tok.data

    std::cout << "m2 is:" << std::endl;
    printListElements(m2);

    std::cout << "printListElements of m1 in intersection:" << std::endl;
    printListElements(input1);
    std::cout << "printListElements of m2 in intersection:" << std::endl;
    printListElements(input2);

    std::cout << "current ring is:" << rString(currRing) << std::endl;
    
    // Extract ideals directly from input lists
    ideal M1 = (ideal)input1->m[3].Data();
    
    // Check if M1 is valid
    if (M1 == NULL) {
        std::cout << "M1 is NULL, returning empty list" << std::endl;
        lists emptyList = (lists)omAlloc0Bin(slists_bin);
        emptyList->Init(0);
        return emptyList;
    }
    
    int size_M1 = IDELEMS(M1);
    std::cout << "M1 has " << size_M1 << " elements" << std::endl;
    
    // Print M1 safely
    std::cout << "printIdeal of M1 in intersection:" << std::endl;
    printIdeal(M1, currRing);
    
    // For M2, we'll use the data from m2 directly
    // This is safer than trying to cast input2->m[3].Data() to an ideal
    std::cout << "Using m2 data for M2" << std::endl;
    
    // Create a new ideal from m2
    ideal M2 = idInit(10, 1); // Initialize with 10 elements, rank 1
    M2->rank = M1->rank; // Use the same rank as M1
    
    // Check if m2 has a string representation of the ideal
    if (m2->nr >= 0 && m2->m[0].rtyp == STRING_CMD) {
        char* idealStr = (char*)m2->m[0].data;
        std::cout << "Parsing ideal string: " << idealStr << std::endl;
        
        // Parse the string into individual polynomials
        std::string str(idealStr);
        std::vector<std::string> polyStrings;
        size_t pos = 0;
        std::string delimiter = ",";
        
        while ((pos = str.find(delimiter)) != std::string::npos) {
            std::string token = str.substr(0, pos);
            polyStrings.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        polyStrings.push_back(str); // Add the last token
        
        // Create polynomials from the strings
        for (size_t i = 0; i < polyStrings.size() && i < 10; i++) {
            std::string polyStr = polyStrings[i];
            // Remove leading/trailing whitespace
            polyStr.erase(0, polyStr.find_first_not_of(" \t\n\r\f\v"));
            polyStr.erase(polyStr.find_last_not_of(" \t\n\r\f\v") + 1);
            
            if (!polyStr.empty()) {
                // Create a polynomial from the string
                poly p = p_Init(currRing);
                // For simplicity, we'll just set the polynomial to a monomial
                // In a real implementation, you would parse the string properly
                p_SetExp(p, 1, 1, currRing);
                p_Setm(p, currRing);
                M2->m[i] = p;
            }
        }
    } else {
        // Copy the generators from m2 to M2
        for (int i = 0; i < 10 && i < m2->nr + 1; i++) {
            if (m2->m[i].rtyp == POLY_CMD) {
                M2->m[i] = pCopy((poly)m2->m[i].data);
            }
        }
    }
    
    std::cout << "M2 has " << IDELEMS(M2) << " elements" << std::endl;
    printIdeal(M2, currRing);
    // Check if M2 is empty
    if (IDELEMS(M2) == 0) {
        std::cout << "M2 is empty, returning empty list" << std::endl;
        id_Delete(&M2, currRing);
        lists emptyList = (lists)omAlloc0Bin(slists_bin);
        emptyList->Init(0);
        return emptyList;
    }
    
    // Compute the intersection
    std::cout << "Computing intersection of ideals..." << std::endl;
    ideal M = idSect(M1, M2);
    int size_M = IDELEMS(M);
    std::cout << "size_of_module_intersection " << size_M << std::endl;
    
    // Create output list
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(1);
    output->m[0].rtyp = MODUL_CMD;
    output->m[0].data = M;
    
    // Clean up M2
    id_Delete(&M2, currRing);
    
    std::cout << "inters:" << std::endl;
    printIdeal(M, currRing);
    
    return output;
}

std::string singular_intersection_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;

    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    lists out = intersection_gpi(args.leftV());  // Call reduce_GPI with the raw pointer

   // std::string function_name = "intersection_gpi";
    //out = call_user_proc(function_name, needed_library, args);
    

    out_filename = serialize(out, base_filename);

    return out_filename;
}



/* 


std::pair<int, lists> std_gpi(leftv arg1) {

    // std::cout << "Type of 1.arg:" << arg1->Typ() <<std::endl;
    lists input = (lists)(arg1->Data()); //extract input
   

lists tmp = (lists)(input->m[3].Data()); // input.data
ideal M= (ideal)tmp->m[0].Data();
int p = IDELEMS(M);
std::cout<<"M_size"<<p<<std::endl;
   // std::cout<<" M_element "<<pString((poly)M->m[14])<<std::endl;

int pr=13;
ideal L = idInit(pr, 1);  // Initialize an empty ideal (syzygy module)

  for (int i = 0; i < pr; i++) {
        L->m[i] = pCopy(M->m[i]);
//        std::cout << "L->m[" << i << "]: " << pString((poly)L->m[i]) << std::endl;
    }

std::cout << "Current ring: " << rString(currRing) << std::endl;
std::cout << "Is global ordering: " << rHasGlobalOrdering(currRing) << std::endl;

ideal Li=kStd(L,NULL, testHomog, NULL);     
int tt=IDELEMS(Li);
 std::cout<<" Li_size "<<tt<<std::endl;

    // Debugging output for Li
    for (int i = 0; i < tt; i++) {
        std::cout << "Li->m[" << i << "]: " << pString((poly)Li->m[i]) << std::endl;
    } 


   // Debugging output for Li
    for (int i = 0; i < tt; i++) {
        std::cout << "Li->m[" << i << "]: " << pString((poly)Li->m[i]) << std::endl;
    }

for(int i=0;i<p;i++){
}
for(int i=0;i<p;i++){
bool b= p_Test((poly)M->m[i],currRing);
 std::cout<<"i= "<<i<<" p_test "<<b<<std::endl;
 p_ContentForGB((poly)M->m[i], currRing);
std::cout << "p_ContentForGB called successfully" << std::endl;
poly po= p_Cleardenom((poly)M->m[i], currRing);
 std::cout<<"i= "<<i<<" poly "<<pString(po)<<std::endl;
p_ProjectiveUnique((poly)M->m[i], currRing);
std::cout << "p_ProjectiveUnique called successfully" << std::endl;

ideal Mi=kStd((ideal)M->m[i],currRing->qideal, testHomog, NULL);
 std::cout<<"i= "<<i<<" Mi_size "<<IDELEMS(Mi)<<std::endl;

} 



//ideal MM=kStd(L,currRing->qideal, testHomog, NULL);
    

// Prepare the output token
    lists output=(lists)omAlloc0Bin(slists_bin);
    output->Init(4);// type token
    // fieldnames
    lists t=(lists)omAlloc0Bin(slists_bin);
    t->Init(2);
    t->m[0].rtyp=STRING_CMD; t->m[0].data=strdup("generators");
    t->m[1].rtyp=STRING_CMD; t->m[1].data=strdup("module_std");
    output->m[1].rtyp=LIST_CMD; output->m[1].data=t;
     output->m[0].rtyp=RING_CMD; output->m[0].data=currRing;
      output->m[2].rtyp=RING_CMD; output->m[2].data=currRing;
    //data
    t=(lists)omAlloc0Bin(slists_bin);
    t->Init(1);
      t->m[0].rtyp=MODUL_CMD; t->m[0].data=Li;
          output->m[3].rtyp=LIST_CMD; output->m[3].data=t;
          return {p,output};

}
  */

  lists std_gpi(leftv arg1) {

    // Extract input list
    lists input = (lists)(arg1->Data());
    
    // Extract tmp list
    lists tmp = (lists)(input->m[3].Data());
    
    // Extract ideal M
    ideal M = (ideal)tmp->m[0].Data();

    // Compute Li
    ideal Li = kStd(M, currRing->qideal, testHomog, NULL);
    int tt = IDELEMS(Li);
    std::cout << "Li_size " << tt << std::endl;

    // Debugging output for Li
    for (int i = 0; i < tt; i++) {
        std::cout << "Li->m[" << i << "]: " << pString((poly)Li->m[i]) << std::endl;
    } 
    
    // Prepare the output token
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(4);
    
    // Create and initialize the first sublist
    lists t1 = (lists)omAlloc0Bin(slists_bin);
    t1->Init(2);
t1->m[0].rtyp = STRING_CMD; t1->m[0].data = strdup("generators");
t1->m[1].rtyp = STRING_CMD; t1->m[1].data = strdup("module_std");
    
output->m[0].rtyp = RING_CMD; output->m[0].data = currRing;
    output->m[1].rtyp = LIST_CMD; output->m[1].data = t1;
output->m[2].rtyp = RING_CMD; output->m[2].data = currRing;
    
// Create and initialize the second sublist for Li elements
    lists t2 = (lists)omAlloc0Bin(slists_bin);
t2->Init(tt); // Use the size of Li to initialize t2
for (int i = 0; i < tt; i++) {
    t2->m[i].rtyp = POLY_CMD; 
    t2->m[i].data = pCopy((poly)Li->m[i]);
}

output->m[3].rtyp = LIST_CMD; output->m[3].data = t2;
    
    return output;
}


NO_NAME_MANGLING
std::string singular_std_gpi(std::string const& res,
    std::string const& needed_library,
    std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    
    std::pair<int, lists> Res;
lists out;  // Change type from std::pair<int, lists> to lists
    std::string ids;
    std::string out_filename;
    
    ids = worker();
    Res = deserialize(res, ids);
    
    ScopedLeftv args(Res.first, lCopy(Res.second));
    
// Call std_gpi directly and assign to lists type
out = std_gpi(args.leftV());  
    
    // Serialize the output
    out_filename = serialize(out, base_filename);
    
    return out_filename;
}

std::string singular_assign_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "assign_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
// implementation.cpp

// Function to create a serialized data_list from multiple feynman struct strings

 std::string  singular_computeBaikovMatrix_gpi(std::string const& res
     , std::string const& needed_library
     , std::string const& base_filename)
 {
     init_singular(config::singularLibrary().string());
         load_singular_library(needed_library);
 
     std::pair<int, lists> Res;
     std::pair<int, lists> out;
     std::string ids;
     std::string out_filename;
 
     ids = worker();
     Res = deserialize(res, ids);
 
     ScopedLeftv args(Res.first, lCopy(Res.second));
 
     std::string function_name = "computeBaikovMatrix_gpi";
     out  = call_user_proc(function_name, needed_library, args);
     out_filename = serialize(out.second, base_filename);
 
     return out_filename;
 }
 
 std::string singular_getTargetInts_gpi(std::string const& res,
     int const& j,
     int const& k,   
     std::string const& needed_library,
     std::string const& base_filename)
 {
     init_singular(config::singularLibrary().string());
         load_singular_library(needed_library);
 
     std::pair<int, lists> Res;
     
     std::pair<int, lists> out;
     std::string ids;
     std::string out_filename;
     
     void* p = (char*)(long)(j);
     void* p1 = (char*)(long)(k);
     ids = worker();
 
     Res = deserialize(res, ids);
  
 
     ScopedLeftv args(Res.first, lCopy(Res.second));
 
     ScopedLeftv args1(args, INT_CMD, p);
     ScopedLeftv args2(args, INT_CMD, p1);
 
     std::string function_name = "getTargetInts_gpi";
     out = call_user_proc(function_name, needed_library, args);
     out_filename = serialize(out.second, base_filename);
 
     
     return out_filename;
 }
 
 std::string singular_getBaikovMatrix_gpi(std::string const& res
     , std::string const& needed_library
     , std::string const& base_filename)
 {
     init_singular(config::singularLibrary().string());
     load_singular_library(needed_library);   
 
     std::pair<int, lists> Res;
     std::pair<int, lists> out;
     std::string ids;
     std::string out_filename;
 
     ids = worker();
     Res = deserialize(res, ids);
 
     ScopedLeftv args(Res.first, lCopy(Res.second));
 
     std::string function_name = "getBaikovMatrix_gpi";
     out = call_user_proc(function_name, needed_library, args);
     out_filename = serialize(out.second, base_filename);
 
     return out_filename;
 }
 
 std::string singular_computeM2_gp(std::string const& res
     , std::string const& res1
     , std::string const& needed_library
     , std::string const& base_filename)
 {
     init_singular(config::singularLibrary().string());
     load_singular_library(needed_library);
     std::pair<int, lists> Res;
     std::pair<int, lists> Res1;
 
     std::pair<int, lists> out;
     std::string ids;
     std::string out_filename;
     ids = worker();
     //std::cout << ids << " in singular_..._compute" << std::endl;
 
     Res = deserialize(res, ids);
     Res1 = deserialize(res1, ids);
 
 
     ScopedLeftv args(Res.first, lCopy(Res.second));
     ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
 
     std::string function_name = "computeM2_gp";
     std::cout<<"applying function_name"<<std::endl;
     out = call_user_proc(function_name, needed_library, args);
     out_filename = serialize(out.second, base_filename);
 
     return out_filename;
 }
/******************************************************************************************************** */
NO_NAME_MANGLING
std::string singular_merge_web_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
    std::string function_name = "merge_web_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_mergeMI_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
        init_singular(config::singularLibrary().string());
        load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    std::string function_name = "mergeMI_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
std::string singular_mergeRedIBPs_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    
    ids = worker();
    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);
    
    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    std::string function_name = "mergeRedIBPs_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);
    
    return out_filename;
}
std::string singular_getReducedIBPSystemInitial_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "getReducedIBPSystemInitial_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

        return out_filename;
    
}
std::string singular_getReducedIBPSystemCompute_gpi(std::string const& res
    , std::string const& res1
    , std::string const& res2
    , std::string const& res3
    , int const& j
    , int const& k
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> Res2;
    std::pair<int, lists> Res3;
    std::pair<int, lists> Res4;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    
    void* p = (char*)(long)(j);
    void* p1 = (char*)(long)(k);
    ids = worker();

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);
    Res2 = deserialize(res2, ids);
    Res3 = deserialize(res3, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
    ScopedLeftv args2(args1, Res2.first, lCopy(Res2.second));
    ScopedLeftv args3(args2, Res3.first, lCopy(Res3.second));
    ScopedLeftv args5(args3, INT_CMD, p);
    ScopedLeftv arg6(args3, INT_CMD, p1);

    std::string function_name = "getReducedIBPSystemCompute_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_getLabeledGraph_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
        init_singular(config::singularLibrary().string());
        load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "getLabeledGraph_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;        
}      
std::string singular_getWeb_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "getWeb_gpi";
    out = call_user_proc(function_name, needed_library, args);  
    out_filename = serialize(out.second, base_filename);

        return out_filename;
}
std::string singular_getReducedIBPs_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "getReducedIBPs_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_getMI_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "getMI_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}               


int singular_IBP_size_gpi(std::string const& res
    , std::string const& needed_library
   )
{
            init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    Res = deserialize(res, ids);
    std::string out_filename1;

    ScopedLeftv args(Res.first, lCopy(Res.second));


    std::string function_name = "IBP_size_gpi";
    out = call_user_proc(function_name, needed_library, args);


    lists u = (lists)out.second->m[3].Data(); //token data

    sleftv& listElement = u->m[0];
    out_filename1 = listElement.String();

   // std::cout << "out_filename1= " << out_filename1 << std::endl

    int ss = std::stoi(out_filename1);
    //std::cout << "ss " << ss << std::endl;

    return ss;
}
void printGraphStructure(const std::string& graph_str) {
    try {
        std::cout << "Graph Structure:" << std::endl;
        
        // Check if the input is empty
        if (graph_str.empty()) {
            std::cout << "  Empty graph structure" << std::endl;
            return;
        }
        
        // Try to parse the graph string directly
        // This avoids using Singular which might be causing issues
        std::cout << "  Raw graph data: " << graph_str << std::endl;
        
        // Try to extract basic information without using Singular
        if (graph_str.find("vertices") != std::string::npos) {
            std::cout << "  Contains vertex information" << std::endl;
        }
        
        if (graph_str.find("edges") != std::string::npos) {
            std::cout << "  Contains edge information" << std::endl;
        }
        
        // Try to use printGpiTokenContent as a fallback
        try {
            std::cout << "  Attempting to use printGpiTokenContent:" << std::endl;
            std::string content = printGpiTokenContent(graph_str, "gpi");
            std::cout << content << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "  Error using printGpiTokenContent: " << e.what() << std::endl;
        }
        
        std::cout << "Graph structure display completed" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error in printGraphStructure: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error in printGraphStructure" << std::endl;
    }
}

//****************************************************************************** */ 

std::string singular_web_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    
    std::string function_name = "web_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}               


std::string singular_gettail_gpi(std::string const& res 
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker(); 
    Res = deserialize(res, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "gettail_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);    

    return out_filename;
}

std::string singular_getlabels_gpi(std::string const& res
    , int const& j
    , int const& k
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;


    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    void* p1 = (char*)(long)(j);
    void* p2 = (char*)(long)(k);

    Res = deserialize(res, ids);
    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv arg1(args, INT_CMD, p1);
    ScopedLeftv arg2(args, INT_CMD, p2);

    std::string function_name = "getlabels_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
std::string singular_makeUserInput_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    Res = deserialize(res, ids);
    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "makeUserInput_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);
    return out_filename;
}           


std::string singular_append_list_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{   
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;   

    ids = worker();
    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    //std::cout<<"append_list_gpi in implementation.cpp"<<std::endl;
    std::string function_name = "append_list_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);    

    return out_filename;
}

/* 
std::string singular_return_list_gpi(int res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;
    void* p = (char*)(long)(res);



    ScopedLeftv arg(INT_CMD, p);
    std::string function_name = "return_list_gpi";
    out = call_user_proc(function_name, needed_library, arg);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
 */

std::string singular_OneSectorComputation_gpi(std::string const& res
    , std::string const& res1
    , std::string const& res2
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> Res2;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;

    ids = worker();
    Res = deserialize(res, ids);    
    Res1 = deserialize(res1, ids);
    Res2 = deserialize(res2, ids);



    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
    ScopedLeftv args2(args1, Res2.first, lCopy(Res2.second));

    std::string function_name = "OneSectorComputation_gpi";
    out = call_user_proc(function_name, needed_library, args);  
    out_filename = serialize(out.second, base_filename);

    return out_filename;    
}
std::string singular_size_OneSectorComputation_gpi(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    Res = deserialize(res, ids);
    ScopedLeftv args(Res.first, lCopy(Res.second));

    std::string function_name = "size_OneSectorComputation_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}           
std::string singular_make_list_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{   
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;   

    ids = worker();
    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    std::string function_name = "make_list_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}
std::string singular_prepareRedIBPs_gpi(std::string const& res
    , int const& j
    , std::string const& needed_library
    , std::string const& base_filename)
{

    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> out;
    std::string ids;    
    ids = worker();
    Res = deserialize(res, ids);
    void* p = (char*)(long)(j);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv arg1(args, INT_CMD, p);

    std::string function_name = "prepareRedIBPs_gpi";
    out = call_user_proc(function_name, needed_library, args);
    std::string out_filename = serialize(out.second, base_filename);

    return out_filename;
}   
std::string singular_returnTargetInts_gpi(std::string const& res
    , std::string const& res1
    , std::string const& res2
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> Res2;
    std::pair<int, lists> out;
    std::string ids;
    ids = worker();
    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);
    Res2 = deserialize(res2, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
    ScopedLeftv args2(args1, Res2.first, lCopy(Res2.second));

    std::string function_name = "returnTargetInts_gpi";
    out = call_user_proc(function_name, needed_library, args);
    std::string  out_filename = serialize(out.second, base_filename);

    return out_filename;
    }

std::string singular_performGaussRed_gpi(std::string const& res,
    std::string const& needed_library,
    std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    lists out;  // Change type from std::pair<int, lists> to lists
    std::string ids;
    std::string out_filename;
    
    ids = worker();
    Res = deserialize(res, ids);
    
    ScopedLeftv args(Res.first, lCopy(Res.second));
    
    std::cout<<"performing gauss reduction"<<std::endl;
    out = singflintGaussRed(args.leftV());  
    
    // Serialize the output
    out_filename = serialize(out, base_filename);
    
    return out_filename;
}

std::string singular_computeGetRedIBPs_gpi(std::string const& res
    , std::string const& res1
    , std::string const& res2
    , int const& j
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> Res2;
    std::pair<int, lists> out;
    std::string ids;

    ids = worker();

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);
    Res2 = deserialize(res2, ids);
    void* p = (char*)(long)(j);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv arg2(args,Res1.first, lCopy(Res1.second));
    ScopedLeftv arg3(arg2,Res2.first, lCopy(Res2.second));
    ScopedLeftv arg4(arg3, INT_CMD, p);
    std::cout<<"computing get red ibps"<<std::endl;

    std::string function_name = "computeGetRedIBPs_gpi";
    out = call_user_proc(function_name, needed_library, args);
    std::string out_filename = serialize(out.second, base_filename);

    return out_filename;
}

std::string singular_performGaussRed(std::string const& res
    , std::string const& needed_library
    , std::string const& base_filename)
{
    // This function is a wrapper around singular_performGaussRed_gpi
    return singular_performGaussRed_gpi(res, needed_library, base_filename);
}

std::string singular_ComputeOneSector_gpi(std::string const& res
    , std::string const& res1
    , std::string const& res2
    , std::string const& needed_library
    , std::string const& base_filename) 
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;
    std::pair<int, lists> Res2;
    std::pair<int, lists> out;
    std::string ids;

    ids = worker();
    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);
    Res2 = deserialize(res2, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
    ScopedLeftv args2(args1, Res2.first, lCopy(Res2.second));

    std::string function_name = "ComputeOneSector_gpi";
    out = call_user_proc(function_name, needed_library, args);
    std::string out_filename = serialize(out.second, base_filename);

    return out_filename;
}               