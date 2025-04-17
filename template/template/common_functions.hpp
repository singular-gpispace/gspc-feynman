#ifndef COMMON_FUNCTIONS_HPP
#define COMMON_FUNCTIONS_HPP

#pragma once

#include <Singular/libsingular.h>
#include <interface/template_interface.hpp>  // This defines NO_NAME_MANGLING
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <regex>
#include "template/config.hpp"

// Common function declarations with NO_NAME_MANGLING
#ifdef __cplusplus
extern "C" {
#endif

// Function to parse nu values from a string
NO_NAME_MANGLING std::vector<int> parseNuValues(const std::string& str);

// Function to print list elements
NO_NAME_MANGLING void printListElements(lists output);

// Function to print matrix
NO_NAME_MANGLING void printMat(matrix M);

// Function to compute M1
NO_NAME_MANGLING lists computeM1_gpi(leftv arg1);

// Function to compute M2
NO_NAME_MANGLING lists computeM2_gpi(leftv arg);

// Function to compute Feynman integral
NO_NAME_MANGLING lists computeFeynmanIntegral_gpi(leftv arg);

// Function to print graph structure
NO_NAME_MANGLING void printGraphStructure(const std::string& graph_str);

// Function to print ideal
NO_NAME_MANGLING void printIdeal(const ideal I, ring R);

// Function to print matrix with ring
NO_NAME_MANGLING void printMatrix(const matrix m, ring R);

// Function to print lists with ring
NO_NAME_MANGLING void printlists(lists l, ring RB);

// Function to print pair int
NO_NAME_MANGLING void printPairInt(lists L);

// Function to print number
NO_NAME_MANGLING void printNumber(const number z, ring R);

// Function to get absolute value
NO_NAME_MANGLING number absValue(poly p, ring R);

// Function to convert number to string
NO_NAME_MANGLING std::string numberToString(number c, ring R);

// Function for Gauss reduction with pivot
NO_NAME_MANGLING lists gaussred_pivot(matrix A);

// Function for intersection
NO_NAME_MANGLING lists intersection_gpi(leftv arg1);

// Function for standard basis
NO_NAME_MANGLING lists std_gpi(leftv arg1);

// Function for Gauss reduction
NO_NAME_MANGLING lists performGaussRed(leftv args);

#ifdef __cplusplus
}
#endif

#endif // COMMON_FUNCTIONS_HPP
