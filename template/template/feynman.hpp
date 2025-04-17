// feynman.h
#ifndef FEYNMAN_HPP
#define FEYNMAN_HPP

#include <Singular/libsingular.h>
#include <flint/fmpq_mat.h>
#include <flint/fmpq.h>
#include <flint/fmpz.h>
#include "common_functions.hpp"
#include <string>
#include <vector>
#include <list>

// Function declarations for FLINT-based Gaussian reduction
lists gaussred_pivot_flint(matrix A);
lists singflintGaussRed(leftv args);

NO_NAME_MANGLING
#endif // FEYNMAN_HPP