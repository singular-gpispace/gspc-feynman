#include <Singular/libsingular.h>


#include <interface/template_interface.hpp>
#include <interface/WorkflowResult.hpp>
#include <interface/ValuesOnPorts.hpp>
#include <interface/Workflow.hpp>
#include <stdexcept>
#include <unistd.h>
#include <kernel/linear_algebra/linearAlgebra.h>

#include "config.hpp"
#include "singular_functions.hpp"
#include <chrono>
#include <tuple>
//#include "Singular/lists.h"
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


// Convert a number to a string for debugging
std::string numberToString(number c, ring R) {
    StringSetS("");              // Initialize string buffer
    n_Write(c, R->cf);           // Write number into buffer
    char* singularStr = StringEndS(); // Retrieve buffer content
    std::string result(singularStr);  // Convert to std::string
    omFree(singularStr);              // Free Singular-allocated memory
    return result;
}

// Print a number as a polynomial for debugging
void printNumber(const number z, ring R) {
    if (n_IsZero(z, R->cf)) {
        std::cout << "number = 0\n";
    } else {
        poly p = p_One(R);
        p_SetCoeff(p, n_Copy(z, R->cf), R);
        p_Setm(p, R);
        std::cout << "number = " << p_String(p, R) << "\n";
        p_Delete(&p, R);
    }
}

// Print a matrix for debugging
void printMatrix(const matrix m, ring R) {
    int rr = MATROWS(m);
    int cc = MATCOLS(m);
    std::cout << "\n-------------\n";
    for (int r = 1; r <= rr; r++) {
        for (int c = 1; c <= cc; c++) {
            char* str = p_String(MATELEM(m, r, c), R);
            std::cout << str << "  ";
            omFree(str);
        }
        std::cout << "\n";
    }
    std::cout << "-------------\n";
}

// Compute the absolute value of a polynomial's coefficient
number absValue(poly p, ring R) {
    if (p == NULL) return n_Init(0, R->cf);
    number result = n_Copy(p_GetCoeff(p, R), R->cf);
    if (!n_GreaterZero(result, R->cf)) {
        number neg = n_InpNeg(result, R->cf);
        n_Delete(&result, R->cf);
        result = neg;
    }
    return result;
}



lists gaussred_pivot(matrix A) {
    ring r = currRing;
    if (!r) {
        std::cerr << "[ERROR] No current ring defined.\n";
        return NULL;
    }

    int n = MATROWS(A), m = MATCOLS(A);
    int mr = (n < m) ? n : m;
    int k = 0, rank = 0;

    matrix P = mpNew(n, n);
    matrix U = mpNew(n, n);
    matrix S = mp_Copy(A, r);

    for (int i = 1; i <= n; i++) {
        MATELEM(P, i, i) = pISet(1);
        MATELEM(U, i, i) = pISet(1);
    }

    for (int i = 1; i <= mr; i++) {
        int col = i + k;
        if (col > m) break;

        int jp = i;
        number max_coeff = n_Copy(absValue(MATELEM(S, i, col), r), r->cf);

        for (int j = i + 1; j <= n; j++) {
            number c = n_Copy(absValue(MATELEM(S, j, col), r), r->cf);
            if (n_Greater(c, max_coeff, r->cf)) {
                n_Delete(&max_coeff, r->cf);
                max_coeff = c;
                jp = j;
        } else {
                n_Delete(&c, r->cf);
            }
        }
        n_Delete(&max_coeff, r->cf);


        if (jp != i) {
            for (int j = 1; j <= m; j++)
                std::swap(MATELEM(S, i, j), MATELEM(S, jp, j));
            for (int j = 1; j <= n; j++)
                std::swap(MATELEM(P, i, j), MATELEM(P, jp, j));
        }

        poly pivot = MATELEM(S, i, col);
        if (!pivot) {
            k++;
            i--;
            continue;
        }

        number pivot_coeff = p_GetCoeff(pivot, r);

        for (int j = i + 1; j <= n; j++) {
            poly num = MATELEM(S, j, col);
            if (!num) {
                continue;
            }

            number num_coeff = p_GetCoeff(num, r);
            number c = n_Div(num_coeff, pivot_coeff, r->cf);
           

            for (int l = col; l <= m; l++) {
                poly s_il = pCopy(MATELEM(S, i, l));
                poly s_jl = pCopy(MATELEM(S, j, l));
             

                poly prod = s_il ? p_Mult_nn(s_il, c, r) : NULL;


                poly sub_result = NULL;

                if (s_jl && prod) {
                    sub_result = p_Sub(p_Copy(s_jl, r), p_Copy(prod, r), r);
                } else if (s_jl && !prod) {
                    sub_result = p_Copy(s_jl, r);
                } else if (!s_jl && prod) {
                    sub_result = p_Neg(p_Copy(prod, r), r);
                }


                if (MATELEM(S, j, l)) p_Delete(&MATELEM(S, j, l), r);
                MATELEM(S, j, l) = sub_result;

                if (prod) p_Delete(&prod, r);
            }

            if (MATELEM(S, j, col)) {
                p_Delete(&MATELEM(S, j, col), r);
                MATELEM(S, j, col) = NULL;
            }

            if (MATELEM(U, j, i)) {
                p_Delete(&MATELEM(U, j, i), r);
                MATELEM(U, j, i) = NULL;
            }
            MATELEM(U, j, i) = p_NSet(c, r);
        }

        rank = i;
    }

    lists Z = (lists)omAlloc0Bin(slists_bin);
    Z->Init(4);
    Z->m[0].rtyp = MATRIX_CMD; Z->m[0].data = P;
    Z->m[1].rtyp = MATRIX_CMD; Z->m[1].data = U;
    Z->m[2].rtyp = MATRIX_CMD; Z->m[2].data = S;
    Z->m[3].rtyp = INT_CMD;    Z->m[3].data = (void*)(long)rank;

    return Z;
}

void printPairInt(lists L) {
    int size = lSize(L) + 1;
    for(int i = 0; i < size; i++) {
        lists pair = (lists)L->m[i].data;
        if (pair) {
            int pi = (int)(long)pair->m[0].data;
            int pj = (int)(long)pair->m[1].data;
            std::cout << "[" << pi << "," << pj << "]" << std::endl;
        }
    }
}

lists computeM1_gpi(leftv arg1) {
    std::cout << "[DEBUG] Starting computeM1_gpi" << std::endl;
    
    lists input = (lists)(arg1->Data());
    lists graphList = (lists)(input->m[3].Data());
    
    // Get rings and matrix
    ring R = (ring)graphList->m[1].Data();
    ring RB = (ring)graphList->m[5].Data();
    matrix B = (matrix)graphList->m[6].Data();

    // Save current ring and switch to RB
    ring savedRing = currRing;
    std::cout << "savedRing=" << rString(savedRing) << std::endl;
    rChangeCurrRing(RB);
    std::cout << "RB=" << rString(currRing) << std::endl;

    int m = rPar(R);  // Number of parameters
    int n = rVar(RB); // Number of variables
    int E = m;        // External momenta
    int L = rVar(R);  // Number of variables of R

    //std::cout << "L=" << L << " E=" << E << " n=" << n << std::endl;
    
    // Create matrix C for derivatives
    matrix C = mpNew(n, n);
    int tem = 0;
    lists x = (lists)omAlloc0Bin(slists_bin);
    x->Init(n);

    // First set of derivatives: external momenta with loop momenta
    for(int i = 1; i <= E; i++) {
        for(int j = E+1; j <= E+L; j++) {
            tem++;
            lists pair = (lists)omAlloc0Bin(slists_bin);
            pair->Init(2);
            pair->m[0].rtyp = INT_CMD;
            pair->m[0].data = (void*)(long)i;
            pair->m[1].rtyp = INT_CMD;
            pair->m[1].data = (void*)(long)j;
            x->m[tem-1].rtyp = LIST_CMD;
            x->m[tem-1].data = pair;
            
            for(int k = 1; k <= n; k++) {
                poly bij = MATELEM(B, i, j);
                MATELEM(C, tem, k) = p_Diff(p_Copy(bij, RB), k, RB);
            }
        }
    }
    //std::cout<<"printing first set of derivatives"<<std::endl;
    //printMatrix(C,RB);

    // Second set: loop momenta with themselves
    for(int i = E+1; i <= E+L; i++) {
        for(int j = i; j <= E+L; j++) {
            tem++;
            lists pair = (lists)omAlloc0Bin(slists_bin);
            pair->Init(2);
            pair->m[0].rtyp = INT_CMD;
            pair->m[0].data = (void*)(long)i;
            pair->m[1].rtyp = INT_CMD;
            pair->m[1].data = (void*)(long)j;
            x->m[tem-1].rtyp = LIST_CMD;
            x->m[tem-1].data = pair;
            
            for(int k = 1; k <= n; k++) {
                poly bij = MATELEM(B, i, j);
                MATELEM(C, tem, k) = p_Diff(p_Copy(bij, RB), k, RB);
            }
        }
    }
    //std::cout<<"printing second set of derivatives"<<std::endl;
    //printMatrix(C,RB);

    // Compute inverse using LU decomposition
    matrix D = mpNew(n, n);
    luInverse(C, D, RB);

    //std::cout<<"printing inverse of matrix C"<<std::endl;
    //printMatrix(D,RB);

    //std::cout<<"size(x)="<<lSize(x)+1<<std::endl;
    //std::cout<<"printing x"<<std::endl;
    //printPairInt(x);
    // Create module M
    ideal M = idInit(n + 1, 1);
    M->rank = n + 1;
    tem = 1;  // Start from 1 like in feynman.lib
    std::cout<<"starting to compute module M"<<std::endl;
    for(int i=E+1; i<=E+L; i++) {
        for(int j=1; j<=E+L; j++) {
            //std::cout << "i=" << i << " j=" << j << std::endl;
            
            // Initialize t as empty vector
            poly t = NULL;
            
            for(int l=1; l<=n; l++) {
                poly a = NULL;
                for(int k=1; k<=E+L; k++) {
                    int o = 0;
                    int c1 = (i == k) ? 2 : 1;
                    poly c2 = NULL;
                    
                    // Find the right row corresponding to derivative
                    for(int r=1; r<=lSize(x)+1; r++) {
                        lists pair = (lists)x->m[r-1].data;
                        if (!pair) continue;
                        int pi = (int)(long)pair->m[0].data;
                        int pj = (int)(long)pair->m[1].data;
                        if ((pi == i && pj == k) || (pi == k && pj == i)) {
                            o = r;
                            break;
                        }
                    }
                    
                    if (o != 0) {
                        c2 = MATELEM(D, l, o);
                        //std::cout << "c2=D[" << l << "," << o << "]=" << p_String(c2, RB) << std::endl;
                        
                        poly bjk = MATELEM(B, j, k);
                        //std::cout << "B[" << j << "," << k << "]=" << p_String(bjk, RB) << std::endl;
                        
                        // Calculate c1*c2*B[j,k] without generator
                        if (c2 && bjk) {
                            poly term = p_Copy(bjk, RB);
                            term = p_Mult_nn(term, n_Init(c1, RB->cf), RB);
                            term = p_Mult_q(term, p_Copy(c2, RB), RB);
                            // Add generator component to term
                            p_Setm(term, RB);
                            a = p_Add_q(a, term, RB);
                        }
                       // std::cout << "a=" << p_String(a, RB) << std::endl;
                    }
                }
                
                // Add a to t (a already has generator component)
                if (a) {
                    poly a1=p_One(RB);
                    p_SetComp(a1, l, RB);
                    p_Setm(a1, RB);
                    poly aa=p_Mult_q(a, a1, RB);
                    t = p_Add_q(t, aa, RB);
                   // std::cout<<"t after adding a="<<p_String(t, RB)<<std::endl;
                }
            }
            
            // Add -2*gen(n+1) if i==j
            if (i == j) {
                poly term = p_ISet(-2, RB);
                p_SetComp(term, n+1, RB);
                p_Setm(term, RB);
                t = p_Add_q(t, term, RB);
            }
            
            //std::cout << "vector t=" << p_String(t, RB) << std::endl;
            M->m[tem-1] = t;
            //std::cout << "M[" << tem << "]=" << p_String(M->m[tem-1], RB) << std::endl;
            tem++;
        }
    }
    std::cout << "size M in M1" << IDELEMS(M) << std::endl;
    for(int i=1; i<=IDELEMS(M); i++) {
        std::cout << "M[" << i << "]=" << p_String(M->m[i-1], RB) << std::endl;
    }
     // Prepare the output token
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(4);
    
    // Create and initialize the first sublist
    lists t1 = (lists)omAlloc0Bin(slists_bin);
    t1->Init(2);
    t1->m[0].rtyp = STRING_CMD; t1->m[0].data = strdup("generators");
    t1->m[1].rtyp = STRING_CMD; t1->m[1].data = strdup("module_M1");
    
    output->m[0].rtyp = RING_CMD; output->m[0].data = currRing;
    output->m[1].rtyp = LIST_CMD; output->m[1].data = t1;
    output->m[2].rtyp = RING_CMD; output->m[2].data = currRing;
    
// Create and initialize the second sublist for Li elements
    lists t2 = (lists)omAlloc0Bin(slists_bin);
    t2->Init(IDELEMS(M)); // Use the size of Li to initialize t2
    for (int i = 0; i < IDELEMS(M); i++) {
        t2->m[i].rtyp = POLY_CMD; 
        t2->m[i].data = pCopy((poly)M->m[i]);
    }

    output->m[3].rtyp = LIST_CMD; output->m[3].data = t2;
    
    // Clean up
    mp_Delete(&C, RB);
    mp_Delete(&D, RB);
    rChangeCurrRing(savedRing);

    return output;
}

std::string singular_computeM1_gpi(std::string const& res,
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
out = computeM1_gpi(args.leftV());  
    
    // Serialize the output
    out_filename = serialize(out, base_filename);
    
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

    ScopedLeftv args5(args, INT_CMD, p);
    ScopedLeftv arg6(args, INT_CMD, p1);

    std::string function_name = "getTargetInts_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    
    return out_filename;
}

void printlists(lists l, ring RB) {
    for (int i = 0; i < lSize(l)+1; i++) {
        std::cout << "l[" << i << "]=" << p_String((poly)l->m[i].Data(), RB) << std::endl;
    }
}

// Print an ideal for debugging
void printIdeal(const ideal I)
{
    if (!I) {
        std::cout << "Ideal is NULL" << std::endl;
        return;
    }
    
    ring savedRing = currRing;
    
    std::cout << "Ideal with " << IDELEMS(I) << " elements:" << std::endl;
    for (int i = 0; i < IDELEMS(I); i++) {
        poly p = I->m[i];
        if (p) {
            char* pStr = p_String(p, currRing);
            std::cout  << (pStr ? pStr : "null") << ",";
            if (pStr) omFree(pStr);
        }
        else {
            std::cout << " 0" << ",";
        }
    }
    std::cout<<std::endl;
    rChangeCurrRing(savedRing);
}
std::vector<int> parseNuValues(const std::string& nuString) {
    std::vector<int> nuValues;
    std::string token;
    std::string delimiter = ",";
    size_t pos = 0;
    std::string s = nuString;

    std::cout << "Parsing nu string: " << s << std::endl;

    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        nuValues.push_back(std::stoi(token));
        s.erase(0, pos + delimiter.length());
    }
    if (!s.empty()) {
        nuValues.push_back(std::stoi(s));
    }

  /*   std::cout << "Parsed " << nuValues.size() << " values" << std::endl;
    for (size_t i = 0; i < nuValues.size(); i++) {
        std::cout << "nuValues[" << i << "]=" << nuValues[i] << std::endl;
    }
 */
    return nuValues;
}
lists computeM2_gpi(leftv arg) {
    std::cout << "DEBUG: Starting computeM2_gpi" << std::endl;
    
    if (!arg || !arg->Data()) {
        std::cout << "DEBUG: Error - arg or arg->Data is NULL" << std::endl;
        return NULL;
    }
    
    lists graphList1 = (lists)(arg->Data());
    if (!graphList1) {
        std::cout << "DEBUG: Error - graphList1 is NULL" << std::endl;
        return NULL;
    }
    std::cout << "DEBUG: graphList1 size: " << graphList1->nr + 1 << std::endl;
    
    if (!arg->next || !arg->next->Data()) {
        std::cout << "DEBUG: Error - arg->next or arg->next->Data is NULL" << std::endl;
        return NULL;
    }
    
    lists targetInt1 = (lists)(arg->next->Data());
    if (!targetInt1) {
        std::cout << "DEBUG: Error - targetInt1 is NULL" << std::endl;
        return NULL;
    }
    std::cout << "DEBUG: targetInt1 size: " << targetInt1->nr + 1 << std::endl;
    
    if (graphList1->nr < 3 || !graphList1->m[3].Data()) {
        std::cout << "DEBUG: Error - graphList1[3] is invalid" << std::endl;
        return NULL;
    }
    
    lists graphList = (lists)(graphList1->m[3].Data());
    if (!graphList) {
        std::cout << "DEBUG: Error - graphList is NULL" << std::endl;
        return NULL;
    }
    std::cout << "DEBUG: graphList size: " << graphList->nr + 1 << std::endl;
    
    if (targetInt1->nr < 3 || !targetInt1->m[3].Data()) {
        std::cout << "DEBUG: Error - targetInt1[3] is invalid" << std::endl;
        return NULL;
    }
    
    lists targetInt = (lists)(targetInt1->m[3].Data());
    if (!targetInt) {
        std::cout << "DEBUG: Error - targetInt is NULL" << std::endl;
        return NULL;
    }
    std::cout << "DEBUG: targetInt size: " << targetInt->nr + 1 << std::endl;
    
    if (graphList->nr < 5 || !graphList->m[5].Data()) {
        std::cout << "DEBUG: Error - graphList[5] is invalid" << std::endl;
        return NULL;
    }
    
    ring RB = (ring)graphList->m[5].Data();
    if (!RB) {
        std::cout << "DEBUG: Error - RB is NULL" << std::endl;
        return NULL;
    }
    std::cout << "DEBUG: Got ring RB" << std::endl;
    
    // Save current ring and switch to RB
    ring savedRing = currRing;
    std::cout << "savedRing=" << rString(savedRing) << std::endl;
    rChangeCurrRing(RB);
    std::cout << "RB=" << rString(currRing) << std::endl;

    int n = rVar(RB);
    std::cout << "DEBUG: Number of variables in ring: " << n << std::endl;
    
    if (targetInt->nr < 0 || !targetInt->m[0].Data()) {
        std::cout << "DEBUG: Error - targetInt[0] is invalid" << std::endl;
        rChangeCurrRing(savedRing);
        return NULL;
    }
    
    // Parse nu values
    std::string nuString = targetInt->m[0].String();
    std::cout << "DEBUG: nuString: " << nuString << std::endl;
    std::vector<int> nuValues = parseNuValues(nuString);
    std::cout << "DEBUG: Parsed " << nuValues.size() << " nu values" << std::endl;
    
    if (nuValues.size() != static_cast<size_t>(n)) {
        std::cout << "DEBUG: Error - nu values size mismatch. Expected " << n << ", got " << nuValues.size() << std::endl;
        WerrorS("The length of the vector nu must equal to number of Baikov variables");
        rChangeCurrRing(savedRing);
        return NULL;
    }
    
    ideal M2 = idInit(n + 1, 1);
    if (!M2) {
        std::cout << "DEBUG: Error - Failed to initialize M2" << std::endl;
        rChangeCurrRing(savedRing);
        return NULL;
    }
    M2->rank = n + 1;
    
    std::cout << "DEBUG: Filling M2" << std::endl;
    
    for(int i = 1; i <= n; i++) {
        poly term;
        int nuVal = nuValues[i-1];
        std::cout << "DEBUG: Processing i=" << i << ", nuVal=" << nuVal << std::endl;
        
        if (nuVal > 0) {
            term = p_ISet(1, RB);
            if (!term) {
                std::cout << "DEBUG: Error - Failed to create term for i=" << i << std::endl;
                id_Delete(&M2, RB);
                rChangeCurrRing(savedRing);
                return NULL;
            }
            p_SetExp(term, i, 1, RB);
            p_SetComp(term, i, RB);
            p_Setm(term, RB);
        } else {
            term = p_One(RB);
            if (!term) {
                std::cout << "DEBUG: Error - Failed to create term for i=" << i << std::endl;
                id_Delete(&M2, RB);
                rChangeCurrRing(savedRing);
                return NULL;
            }
            p_SetComp(term, i, RB);
            p_Setm(term, RB);
        }
        M2->m[i-1] = term;
    }
    
    poly last_term = p_One(RB);
    if (!last_term) {
        std::cout << "DEBUG: Error - Failed to create last term" << std::endl;
        id_Delete(&M2, RB);
        rChangeCurrRing(savedRing);
        return NULL;
    }
    p_SetComp(last_term, n+1, RB);
    p_Setm(last_term, RB);
    M2->m[n] = last_term;
    
    std::cout << "DEBUG: M2 created successfully" << std::endl;
    printIdeal(M2);
    
    std::cout << "DEBUG: Creating output" << std::endl;
    lists output = (lists)omAlloc0Bin(slists_bin);
    if (!output) {
        std::cout << "DEBUG: Error - Failed to allocate output" << std::endl;
        id_Delete(&M2, RB);
        rChangeCurrRing(savedRing);
        return NULL;
    }
    output->Init(4);
    
    lists t1 = (lists)omAlloc0Bin(slists_bin);
    if (!t1) {
        std::cout << "DEBUG: Error - Failed to allocate t1" << std::endl;
        id_Delete(&M2, RB);
        omFreeBin(output, slists_bin);
        rChangeCurrRing(savedRing);
        return NULL;
    }
    t1->Init(2);
    t1->m[0].rtyp = STRING_CMD; t1->m[0].data = strdup("generators");
    t1->m[1].rtyp = STRING_CMD; t1->m[1].data = strdup("module_M2");
    
    output->m[0].rtyp = RING_CMD; output->m[0].data = RB;
    output->m[1].rtyp = LIST_CMD; output->m[1].data = t1;
    output->m[2].rtyp = RING_CMD; output->m[2].data = RB;
    
    lists t2 = (lists)omAlloc0Bin(slists_bin);
    if (!t2) {
        std::cout << "DEBUG: Error - Failed to allocate t2" << std::endl;
        id_Delete(&M2, RB);
        omFreeBin(output, slists_bin);
        rChangeCurrRing(savedRing);
        return NULL;
    }
    t2->Init(IDELEMS(M2));
    for (int i = 0; i < IDELEMS(M2); i++) {
        t2->m[i].rtyp = POLY_CMD;
        t2->m[i].data = pCopy((poly)M2->m[i]);
    }
    
    output->m[3].rtyp = LIST_CMD; output->m[3].data = t2;
    
    // Restore the original ring
    rChangeCurrRing(savedRing);
    
    return output;
}

std::string singular_computeM2_gpi(std::string const& res
    , std::string const& res1
    , std::string const& needed_library
    , std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::pair<int, lists> Res1;

    std::string ids;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);

    // Create a deep copy of the lists to avoid memory issues
    lists resCopy = lCopy(Res.second);
    lists res1Copy = lCopy(Res1.second);

    // Create ScopedLeftv objects with the copies
    ScopedLeftv args(Res.first, resCopy);
    ScopedLeftv args1(args, Res1.first, res1Copy);

    // Call computeM2_gpi with the raw pointer
    lists out = computeM2_gpi(args.leftV());

    // Create a deep copy of the output before serializing
    lists outCopy = lCopy(out);
    
    // Serialize the copy
    std::string out_filename = serialize(outCopy, base_filename);
    
    // Clean up the copy after serialization

    return out_filename;
}




std::string singular_computeM2(std::string const& res
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

    std::string function_name = "computeM2_gpi1";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

NO_NAME_MANGLING
std::pair<int, lists> intersection_gpi(leftv arg1) {
   

    // Extract input list
    lists input = (lists)(arg1->Data());
   
        // Extract tmp list
    lists tmp = (lists)(input->m[3].Data());

    lists Tok = (lists)(arg1->next->Data()); // extract Tok
    lists tmp1 = (lists)(Tok->m[3].Data()); // Tok.data


  

    // Extract ideal M
    ideal M1 = (ideal)tmp->m[0].Data();
    ideal M2 = (ideal)tmp1->m[0].Data();

    ideal M = idSect(M1, M2);
    int size_M= IDELEMS( M);
    std::cout<<"size_of_module_intersection "<<size_M<<std::endl;
int p=1;
    // Prepare the output token
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(4);

    lists t = (lists)omAlloc0Bin(slists_bin);
    t->Init(2);
    t->m[0].rtyp = STRING_CMD; t->m[0].data = strdup("generators");
    t->m[1].rtyp = STRING_CMD; t->m[1].data = strdup("module_intersection");
    output->m[1].rtyp = LIST_CMD; output->m[1].data = t;
    output->m[0].rtyp = RING_CMD; output->m[0].data = currRing;
    output->m[2].rtyp = RING_CMD; output->m[2].data = currRing;

    t = (lists)omAlloc0Bin(slists_bin);
    t->Init(1); // Use the size of Li to initialize t
        t->m[0].rtyp = MODUL_CMD; t->m[0].data = M;

    output->m[3].rtyp = LIST_CMD; output->m[3].data = t;

    // Cleanup

    return {p, output};
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

    std::pair<int, lists> out;
    std::string ids;
    std::string out_filename;
    ids = worker();
    //std::cout << ids << " in singular_..._compute" << std::endl;

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);


    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));

    out = intersection_gpi(args.leftV());  // Call reduce_GPI with the raw pointer

   // std::string function_name = "intersection_gpi";
    //out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

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

// Define the setIBP structure
typedef struct {
    ring over;
    lists seed;
    lists IBP;
} setIBP;

// Define the oneIBP structure
typedef struct {
    lists c;
    lists i;
} oneIBP;

// Forward declarations for helper functions
lists getSortedIntegrals(setIBP* S);
lists getRandom(int p, int s);
matrix setMat(setIBP* S, lists val, lists ind);
lists gaussred_pivot(matrix A);

// Implementation of getRedIBPs_gpi
lists getRedIBPs_gpi(leftv arg, int p) {
    std::cout << "DEBUG: Starting getRedIBPs_gpi with p=" << p << std::endl;
    
    if (!arg || !arg->Data()) {
        std::cout << "DEBUG: Error - arg or arg->Data is NULL" << std::endl;
        return NULL;
    }
    
    // Extract the setIBP from the token
    lists token = (lists)(arg->Data());
    if (!token || token->nr < 1 || !token->m[0].Data()) {
        std::cout << "DEBUG: Error - Invalid token structure" << std::endl;
        return NULL;
    }
    
    // Extract the setIBP from the token data
    lists tokenData = (lists)(token->m[0].Data());
    if (!tokenData || tokenData->nr < 1 || !tokenData->m[0].Data()) {
        std::cout << "DEBUG: Error - Invalid token data structure" << std::endl;
        return NULL;
    }
    
    // Get the setIBP
    setIBP* S = (setIBP*)(tokenData->m[0].Data());
    if (!S) {
        std::cout << "DEBUG: Error - setIBP is NULL" << std::endl;
        return NULL;
    }
    
    // Save current ring and switch to the ring from setIBP
    ring savedRing = currRing;
    ring R = S->over;
    if (!R) {
        std::cout << "DEBUG: Error - Ring from setIBP is NULL" << std::endl;
        return NULL;
    }
    
    std::cout << "DEBUG: Switching to ring from setIBP" << std::endl;
    rChangeCurrRing(R);
    
    // Get sorted integrals
    lists ind = getSortedIntegrals(S);
    if (!ind) {
        std::cout << "DEBUG: Error - Failed to get sorted integrals" << std::endl;
        rChangeCurrRing(savedRing);
        return NULL;
    }
    
    // Get random values for parameters
    lists val = getRandom(p, rPar(R));
    if (!val) {
        std::cout << "DEBUG: Error - Failed to get random values" << std::endl;
        rChangeCurrRing(savedRing);
        return NULL;
    }
    
    // Create matrix for row reduction
    matrix N = setMat(S, val, ind);
    if (!N) {
        std::cout << "DEBUG: Error - Failed to create matrix" << std::endl;
        rChangeCurrRing(savedRing);
        return NULL;
    }
    
    // Create a ring for finite field computation
    ring RZ = rCopy(R);
    rChangeCurrRing(RZ);
    
    // Map the matrix to the new ring
    matrix NZ = map(R, N);
    
    // Perform row reduction
    lists Z = gaussred_pivot(NZ);
    if (!Z) {
        std::cout << "DEBUG: Error - Failed to perform row reduction" << std::endl;
        rChangeCurrRing(savedRing);
        return NULL;
    }
    
    // Switch back to original ring
    rChangeCurrRing(R);
    
    // Map the result back
    lists ZR = imap(RZ, Z);
    
    // Extract independent IBPs
    lists indIBP = (lists)omAlloc0Bin(slists_bin);
    indIBP->Init(1);
    int l = 1;
    
    for (int j = 1; j <= (int)(long)ZR->m[3].Data(); j++) {
        for (int k = 1; k <= MATCOLS((matrix)ZR->m[0].Data()); k++) {
            if (MATELEM((matrix)ZR->m[0].Data(), j, k) != NULL) {
                indIBP->m[l-1].rtyp = POLY_CMD;
                indIBP->m[l-1].data = pCopy((poly)S->IBP->m[k-1].Data());
                l++;
                indIBP->Init(l);
            }
        }
    }
    
    // Extract seed (indices of non-zero columns)
    lists seed = (lists)omAlloc0Bin(slists_bin);
    seed->Init(1);
    l = 1;
    
    for (int k = 1; k <= MATCOLS((matrix)ZR->m[2].Data()); k++) {
        for (int j = 1; j <= MATROWS((matrix)ZR->m[2].Data()); j++) {
            if (MATELEM((matrix)ZR->m[2].Data(), j, k) != NULL) {
                lists pair = (lists)ind->m[k-1].Data();
                seed->m[l-1].rtyp = INT_CMD;
                seed->m[l-1].data = (void*)(long)((int)(long)pair->m[0].Data());
                l++;
                seed->Init(l);
                break;
            }
        }
    }
    
    // Create output token
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(4);
    
    // Set field names
    lists fieldnames = (lists)omAlloc0Bin(slists_bin);
    fieldnames->Init(2);
    fieldnames->m[0].rtyp = STRING_CMD;
    fieldnames->m[0].data = strdup("generators");
    fieldnames->m[1].rtyp = STRING_CMD;
    fieldnames->m[1].data = strdup("getRedIBPs_gpi");
    
    output->m[0].rtyp = RING_CMD;
    output->m[0].data = currRing;
    output->m[1].rtyp = LIST_CMD;
    output->m[1].data = fieldnames;
    output->m[2].rtyp = RING_CMD;
    output->m[2].data = currRing;
    
    // Set data
    lists data = (lists)omAlloc0Bin(slists_bin);
    data->Init(2);
    data->m[0].rtyp = LIST_CMD;
    data->m[0].data = indIBP;
    data->m[1].rtyp = LIST_CMD;
    data->m[1].data = seed;
    
    output->m[3].rtyp = LIST_CMD;
    output->m[3].data = data;
    
    // Restore original ring
    rChangeCurrRing(savedRing);
    
    return output;
}

// Implementation of getSortedIntegrals
lists getSortedIntegrals(setIBP* S) {
    lists ind = (lists)omAlloc0Bin(slists_bin);
    ind->Init(0);
    
    lists indS = (lists)omAlloc0Bin(slists_bin);
    indS->Init(0);
    
    for (int j = 1; j <= lSize(S->IBP) + 1; j++) {
        oneIBP* oneI = (oneIBP*)(S->IBP->m[j-1].Data());
        for (int k = 1; k <= lSize(oneI->i) + 1; k++) {
            lists indv = (lists)(oneI->i->m[k-1].Data());
            
            if (lSize(ind) == 0) {
                ind->m[0].rtyp = LIST_CMD;
                ind->m[0].data = indv;
                ind->Init(1);
                
                indS->m[0].rtyp = INT_CMD;
                indS->m[0].data = (void*)(long)k;
                indS->Init(1);
            } else {
                // Check if indv is already in indS
                bool found = false;
                for (int i = 0; i < lSize(indS) + 1; i++) {
                    if ((int)(long)indS->m[i].Data() == k) {
                        found = true;
                        break;
                    }
                }
                
                if (!found) {
                    ind->m[lSize(ind)].rtyp = LIST_CMD;
                    ind->m[lSize(ind)].data = indv;
                    ind->Init(lSize(ind) + 1);
                    
                    indS->m[lSize(indS)].rtyp = INT_CMD;
                    indS->m[lSize(indS)].data = (void*)(long)k;
                    indS->Init(lSize(indS) + 1);
                }
            }
        }
    }
    
    return ind;
}

// Implementation of getRandom
lists getRandom(int p, int s) {
    lists L = (lists)omAlloc0Bin(slists_bin);
    L->Init(s);
    
    for (int i = 1; i <= s; i++) {
        L->m[i-1].rtyp = INT_CMD;
        L->m[i-1].data = (void*)(long)(rand() % p);
    }
    
    return L;
}

// Implementation of setMat
matrix setMat(setIBP* S, lists val, lists ind) {
    ring RZ = S->over;
    int rows = lSize(S->IBP) + 1;
    int cols = lSize(ind) + 1;
    
    matrix X = mpNew(rows, cols);
    
    for (int j = 1; j <= rows; j++) {
        oneIBP* L = (oneIBP*)(S->IBP->m[j-1].Data());
        
        for (int k = 1; k <= cols; k++) {
            lists indv = (lists)(ind->m[k-1].Data());
            poly coef = p_One(RZ);
            
            // Use val parameter to set coefficient
            if (val != NULL && k <= lSize(val)) {
                coef = (poly)val->m[k-1].Data();
            }
            
            MATELEM(X, j, k) = coef;
        }
    }
    
    return X;
}
