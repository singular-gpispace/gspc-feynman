
#include <Singular/libsingular.h>


#include <interface/template_interface.hpp>
#include <interface/WorkflowResult.hpp>
#include <interface/ValuesOnPorts.hpp>
#include <interface/Workflow.hpp>
#include <stdexcept>
#include <unistd.h>

#include "config.hpp"
#include "singular_functions.hpp"
#include <chrono>
#include <tuple>
//#include "Singular/lists.h"
#include <typeinfo>

#include <stdio.h>
#include <string>

NO_NAME_MANGLING


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

std::string singular_computeM1_gpi(std::string const& res
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
    std::string function_name = "computeM1_gpi";
    out = call_user_proc(function_name, needed_library, args);
  /*  lists u = (lists)out.second->m[3].Data();
    //  lists u1 = (lists)out.second->m[3].Data(); //token data

       int L_size1 = lSize(u)+1;
        std::cout << "computeM1_gpi " << L_size1 << std::endl;
    for (int i = 0; i < L_size1; i++) {
        sleftv& listElement = u->m[i];  // Access each element as `leftv`
        if (listElement.data == NULL) {
            std::cout << "Input: NULL" << std::endl;
        }

       auto out_filename1 = listElement.String();
         std::cout << "i=" << i << " " << "out_inside= " << out_filename1 << std::endl;


    }   */
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}



std::string singular_computeM2_gpi(std::string const& res
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

    std::string function_name = "computeM2_gpi";
    out = call_user_proc(function_name, needed_library, args);

   
    out_filename = serialize(out.second, base_filename);

    return out_filename;
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

    std::string function_name = "intersection_gpi";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}


#include <singular/Singular/libsingular.h>
#include <iostream>
#include <cstring>
#include "polys/ext_fields/transext.h"

std::pair<int, lists> std_gpi(leftv arg1) {
    if (!arg1 || !arg1->Data()) {
        std::cerr << "Error: Invalid input argument!" << std::endl;
        return {0, nullptr};
    }
    std::cout << "Current first ring: " << rString(currRing) << std::endl;
ring R=currRing;
    // Extract input list
    lists input = (lists)(arg1->Data());
    if (!input || input->nr < 3) {
        std::cerr << "Error: Input list is invalid or too small!" << std::endl;
        return {0, nullptr};
    }

    // Extract tmp list
    lists tmp = (lists)(input->m[3].Data());
    if (!tmp || tmp->nr < 0) {
        std::cerr << "Error: Invalid tmp list!" << std::endl;
        return {0, nullptr};
    }

    // Extract ideal M
    ideal M = (ideal)tmp->m[0].Data();
    if (!M) {
        std::cerr << "Error: Ideal M is null!" << std::endl;
        return {0, nullptr};
    }

    // Check size of M
    int p = IDELEMS(M);
    if (p <= 0) {
        std::cerr << "Error: Ideal M has no elements!" << std::endl;
        return {0, nullptr};
    }

    // Set pr dynamically based on M
    int pr =14; // Use the size of M to determine pr
    if (pr > IDELEMS(M)) {
        std::cerr << "Error: pr exceeds the size of M!" << std::endl;
        return {0, nullptr};
    }

    // Initialize ideal I
    ideal I = idInit(pr, 1);
    if (!I) {
        std::cerr << "Error: Failed to initialize ideal I!" << std::endl;
        return {0, nullptr};
    }

    // Copy polynomials from M to I
    for (int i = 0; i < pr; i++) {
        if (!M->m[i] || !p_Test((poly)M->m[i], currRing)) {
            std::cerr << "Error: M->m[" << i << "] is invalid!" << std::endl;
            return {0, nullptr};
        }
        I->m[i] = pCopy(M->m[i]);
        std::cout << "I->m[" << i << "]: " << pString((poly)I->m[i]) << std::endl;
    }
    char **n = (char**)omalloc(3 * sizeof(char*));
    n[0] = omStrDup("t(1)");
    n[1] = omStrDup("t(2)");
    n[2] = omStrDup("D");

    // Create ring Z/32003[x,y,z]
    ring extRing = rDefault(0, 3, n);

    TransExtInfo extParam;
          extParam.r = extRing;
          coeffs cf = nInitChar(n_transExt, &extParam);


    // Define coefficient variables t(1), t(2)
    const int numVars = 9;       // z(1) to z(9)

   

    // Dynamically create the names for the variables z(1) to z(9)
    char *vars[numVars];
    for (int i = 0; i < numVars; ++i) {
        vars[i] = (char*)omAlloc(5 * sizeof(char)); // "z(i)" where i = 1,2,...,9
        sprintf(vars[i], "z(%d)", i + 1);
    }

    // Define the orders (dp(9), C)
    rRingOrder_t* order = (rRingOrder_t*)omAlloc0(3 * sizeof(rRingOrder_t));
    order[0] = ringorder_dp;  // dp(9)
    order[1] = ringorder_c;   // C (for the coefficient field)

    // Define the blocks of the ring
    int* block0 = (int*)omAlloc(3 * sizeof(int));
    block0[0] = 1;  
    int* block1 = (int*)omAlloc0(3 * sizeof(int));
    block1[0] = 9;  


    // Define the ring using rDefault function
    ring r = rDefault(cf, numVars,vars,3, order, block0, block1);  // 0 means coefficient field is ℚ
    if (!r) {
        std::cerr << "Error: Failed to create polynomial ring!" << std::endl;
       
        for (int i = 0; i < numVars; ++i) {
            omFree(vars[i]);
        }
        omFree(order);
        omFree(block0);
        omFree(block1);
        return {0, nullptr};
    }

    std::cout << "New ring: " << rString(r) << std::endl;

    // Define the image ideal dynamically using a loop
    ideal image_id = idInit(numVars, 1);
    for (int i = 0; i < numVars; ++i) {
        image_id->m[i] = p_ISet(1, r);
        p_SetExp(image_id->m[i], i + 1, 1, r);  // a, b, etc.
        p_Setm(image_id->m[i], r);
    }

    // Use identity coefficient mapping
    nMapFunc coeff_map = n_SetMap(R->cf, r->cf);

    // Perform the ideal mapping
    ideal J = maMapIdeal(I, R, image_id, r, coeff_map);

    std::cout << "Ideal mapped to new ring r." << std::endl;

    for (int i = 0; i < IDELEMS(J); i++) {
        std::cout << "J[" << i << "] = " << pString(J->m[i]) << std::endl;
    }
        // Change the current ring
    rChangeCurrRing(r);
    // Compute the Gröbner basis using the new ring
    ideal G = kStd(J, NULL, testHomog, NULL);
    if (!G) {
        std::cerr << "Error: Failed to compute Gröbner basis!" << std::endl;
        id_Delete(&I, r);
        rDelete(r);
        return {0, nullptr};
    }

    std::cout << "Gröbner basis computed." << std::endl;

    // Print the size of the Gröbner basis
    int tt = IDELEMS(G);
    std::cout << "Gröbner basis has " << tt << " elements:" << std::endl;

    // Print the Gröbner basis
    for (int i = 0; i < tt; i++) {
        char* polyStr = p_String(G->m[i], r);
        if (polyStr) {
            std::cout << "G[" << i << "]: " << polyStr << std::endl;
            omFree(polyStr); // Free memory
        } else {
            std::cerr << "Failed to convert polynomial to string" << std::endl;
        }
    }

    // Prepare the output token
    lists output = (lists)omAlloc0Bin(slists_bin);
    if (!output) {
        std::cerr << "Error: Failed to allocate memory for output!" << std::endl;
        id_Delete(&I, r);
        id_Delete(&G, r);
        rDelete(r);
        return {0, nullptr};
    }

    output->Init(4);

    lists t = (lists)omAlloc0Bin(slists_bin);
    t->Init(2);
    t->m[0].rtyp = STRING_CMD; t->m[0].data = (void*)strdup("generators");
    t->m[1].rtyp = STRING_CMD; t->m[1].data = (void*)strdup("module_std");
    output->m[1].rtyp = LIST_CMD; output->m[1].data = t;

    output->m[0].rtyp = RING_CMD; output->m[0].data = (void*)r;
    output->m[2].rtyp = RING_CMD; output->m[2].data = (void*)r;

    t = (lists)omAlloc0Bin(slists_bin);
    t->Init(tt); // Use the size of G to initialize t
    for (int i = 0; i < tt; i++) {
        t->m[i].rtyp = POLY_CMD; t->m[i].data = (void*)p_Copy(G->m[i], r);
    }
    output->m[3].rtyp = LIST_CMD; output->m[3].data = t;

    // Cleanup
    id_Delete(&I, r); // Cleanup the ideal
    id_Delete(&G, r); // Cleanup the Gröbner basis
    rDelete(r); // Cleanup the ring

    return {tt, output};
}

std::string singular_std_gpi(std::string const& res
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
  /*
    lists Token = (lists)(args.leftV()->data);
std::string  out_filename1;
   int L_size = lSize(Token) + 1;
    for (int i = 0; i < L_size; i++) {
        sleftv& listElement = Token->m[i];  // Access each element as `leftv`
        if (listElement.data == NULL) {
         std::cout << "Input: NULL" << std::endl;
        }
        else if (i == 3) {
             out_filename1 = listElement.String();
        std::cout << "out_filename1= " << out_filename1 << std::endl;
        }
    }  */
    /* std::string function_name = "std_gpi";
    out = call_user_proc(function_name, needed_library, args); */
    out = std_gpi(args.leftV());  // Call reduce_GPI with the raw pointer


    out_filename = serialize(out.second, base_filename);

    return out_filename;
}

