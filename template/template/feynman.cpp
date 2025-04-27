#include <Singular/libsingular.h>
#include <interface/template_interface.hpp>  // This defines NO_NAME_MANGLING
#include "feynman.hpp"
#include "singular_functions.hpp"
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <regex>
#include "template/config.hpp"
#include <kernel/linear_algebra/linearAlgebra.h>
#include <singular/Singular/maps_ip.h>
#include "polys/ext_fields/transext.h"
#include "polys/flintconv.h"  // Add this for FLINT conversion functions
// Add FLINT headers
#include <flint/fmpq_mat.h>
#include <flint/fmpq.h>
#include <flint/fmpz.h>
#include <flint/fq_mat.h>
#include <flint/fq_nmod.h>
#include <filesystem>

// Add type definitions at the top of the file, after the includes
typedef struct {
    lists i;  // indices
    lists c;  // coefficients
} *oneIBP;

typedef struct {
    oneIBP* IBP;  // array of IBPs
    ring over;    // ring over which the IBPs are defined
} *setIBP;

// Define SETIBP_CMD if not already defined
#ifndef SETIBP_CMD
#define SETIBP_CMD 12345  // Use a unique number that doesn't conflict with existing commands
#endif


// Implementation of parseNuValues function with NO_NAME_MANGLING
#ifdef __cplusplus
extern "C" {
#endif

NO_NAME_MANGLING std::vector<int> parseNuValues(const std::string& str) {
    std::vector<int> values;
    std::string cleaned = str;
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '['), cleaned.end());
    cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), ']'), cleaned.end());
    std::stringstream ss(cleaned);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            values.push_back(std::stoi(item));
        }
    }
    return values;
}

#ifdef __cplusplus
}
#endif

#include <cstring>

void printlist(const std::string & tokenString, const std::string & needed_library)
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
}
// Implementation of printListElements function
void printListElements(lists output) {
    if (!output) {
        std::cerr << "ERROR: Null list in printListElements" << std::endl;
        return;
    }
    
    std::cout << "List with " << output->nr + 1 << " elements:" << std::endl;
    for (int i = 0; i <= output->nr; i++) {
        std::cout << "  [" << i << "]: ";
        if (output->m[i].rtyp == STRING_CMD) {
            std::cout << "STRING: " << (char*)output->m[i].data << std::endl;
        } else if (output->m[i].rtyp == INT_CMD) {
            std::cout << "INT: " << (int)(long)output->m[i].data << std::endl;
        } else if (output->m[i].rtyp == LIST_CMD) {
            std::cout << "LIST" << std::endl;
            printListElements((lists)output->m[i].data);
        } else if (output->m[i].rtyp == RING_CMD) {
            std::cout << "RING: " << rString((ring)output->m[i].data) << std::endl;
        } else if (output->m[i].rtyp == MATRIX_CMD) {
            std::cout << "MATRIX" << std::endl;
            printMat((matrix)output->m[i].data);
        } else {
            std::cout << "Type: " << output->m[i].rtyp << std::endl;
        }
    }
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
void printlists(lists l, ring RB) {
    for (int i = 0; i < lSize(l)+1; i++) {
        std::cout << "l[" << i << "]=" << p_String((poly)l->m[i].Data(), RB) << std::endl;
    }
}

// Print an ideal for debugging
void printIdeal(const ideal I,ring R)
{
    if (!I) {
        std::cout << "Ideal is NULL" << std::endl;
        return;
    }
    
    
    std::cout << "Ideal with " << IDELEMS(I) << " elements:" << std::endl;
    for (int i = 0; i < IDELEMS(I); i++) {
        poly p = I->m[i];
        if (p) {
            char* pStr = p_String(p, R);
            std::cout  << (pStr ? pStr : "null") << ",";
            if (pStr) omFree(pStr);
        }
        else {
            std::cout << " 0" << ",";
        }
    }
    std::cout<<std::endl;
}
// Implementation of printMat function
void printMat(matrix M) {
    if (!M) {
        std::cerr << "ERROR: Null matrix in printMat" << std::endl;
        return;
    }
    
    int rr = MATROWS(M);
    int cc = MATCOLS(M);
    std::cout << "\n-------------\n";
    for (int r = 1; r <= rr; r++) {
        for (int c = 1; c <= cc; c++) {
            char* str = p_String(MATELEM(M, r, c), currRing);
            std::cout << str << "  ";
            omFree(str);
        }
        std::cout << "\n";
    }
    std::cout << "-------------\n";
}


lists computeM1_gpi(leftv arg1) {
    std::cout << "[DEBUG] Starting computeM1_gpi" << std::endl;
    
    lists input = (lists)(arg1->Data());
    lists graphList = (lists)(input->m[3].Data());
    
    ring savedRing = currRing;
    std::cout << "savedRing=" << rString(savedRing) << std::endl;
    // Get rings and matrix
    ring R = (ring)graphList->m[1].Data();
    ring RB = (ring)graphList->m[5].Data();
    matrix B = (matrix)graphList->m[6].Data();

    // Save current ring and switch to RB
  
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
                        //stMd::cout << "B[" << j << "," << k << "]=" << p_String(bjk, RB) << std::endl;
                        
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
    output->m[3].rtyp = MODUL_CMD; output->m[3].data = idCopy(M);
    
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

lists computeM2_gpi(leftv arg) {
    // Validate input
    if (!arg || !arg->Data() || !arg->next || !arg->next->Data()) {
        std::cerr << "ERROR: Invalid arguments" << std::endl;
        return nullptr;
    }

    // Get labeledgraph (check for token wrapper)
    lists graphList1 = (lists)(arg->Data());
    lists graphList = graphList1;
    if (graphList1->nr >= 3 && graphList1->m[3].rtyp == LIST_CMD && graphList1->m[3].Data()) {
        graphList = (lists)(graphList1->m[3].Data());
    }

    // Debug: Print graphList
    std::cout << "DEBUG: graphList contents:" << std::endl;
    printListElements(graphList);

    // Validate labeledgraph
    if (!graphList || graphList->nr < 7) { // labeledgraph has 8 fields
        std::cerr << "ERROR: Invalid labeledgraph (expected 8 fields, got " 
                  << (graphList ? graphList->nr + 1 : 0) << ")" << std::endl;
        return nullptr;
    }

    // Get target integrals (check for token wrapper)
    lists targetInt1 = (lists)(arg->next->Data());
    lists targetInt = targetInt1;
    if (targetInt1->nr >= 3 && targetInt1->m[3].rtyp == LIST_CMD && targetInt1->m[3].Data()) {
        targetInt = (lists)(targetInt1->m[3].Data());
    }

    // Debug: Print targetInt
    std::cout << "DEBUG: targetInt contents:" << std::endl;
    printListElements(targetInt);

    // Validate target integrals
    if (!targetInt || targetInt->nr < 0 || !targetInt->m[0].Data()) {
        std::cerr << "ERROR: Invalid target integrals" << std::endl;
        return nullptr;
    }

    // Extract rings
    ring RB = nullptr;
    ring baikovRing = nullptr;
    
    // According to the debug output, the rings are in the labeledgraph structure
    // labeledgraph = (list vertices, list edges, ring over, list labels, ring overpoly, list elimvars, ring baikovover, matrix baikovmatrix)
    
    // Extract the 'over' ring (position 2)
    if (graphList->m[2].rtyp == RING_CMD && graphList->m[2].Data()) {
        RB = (ring)graphList->m[2].Data();
        std::cout << "DEBUG: Found over ring: " << rString(RB) << std::endl;
    }
    
    // Extract the 'baikovover' ring (position 6)
    if (graphList->m[6].rtyp == RING_CMD && graphList->m[6].Data()) {
        baikovRing = (ring)graphList->m[6].Data();
        std::cout << "DEBUG: Found baikovover ring: " << rString(baikovRing) << std::endl;
    }
    
    if (!RB || !baikovRing) {
        std::cerr << "ERROR: Missing rings (over=" << (RB ? "present" : "missing") 
                  << ", baikovover=" << (baikovRing ? "present" : "missing") << ")" << std::endl;
        return nullptr;
    }

    // Switch to RB ring
    ring savedRing = currRing;
    rChangeCurrRing(RB);

    // Get number of variables
    int n = rVar(RB);

    // Parse nu values
    std::string nuString = targetInt->m[0].String();
    std::vector<int> nuValues = parseNuValues(nuString);

    // Create M2 ideal
    ideal M2 = idInit(n + 1, 1);
    M2->rank = n + 1;

    for (int i = 1; i <= n; i++) {
        poly term = nuValues[i-1] > 0 ? p_ISet(1, RB) : p_One(RB);
        if (nuValues[i-1] > 0) {
            p_SetExp(term, i, 1, RB);
        }
        p_SetComp(term, i, RB);
        p_Setm(term, RB);
        M2->m[i-1] = term;
    }

    poly last_term = p_One(RB);
    p_SetComp(last_term, n + 1, RB);
    p_Setm(last_term, RB);
    M2->m[n] = last_term;

    // Create output list
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(4);

    lists t1 = (lists)omAlloc0Bin(slists_bin);
    t1->Init(2);
    t1->m[0].rtyp = STRING_CMD; t1->m[0].data = omStrDup("generators");
    t1->m[1].rtyp = STRING_CMD; t1->m[1].data = omStrDup("module_M2");

    output->m[0].rtyp = RING_CMD; output->m[0].data = currRing;
    output->m[1].rtyp = LIST_CMD; output->m[1].data = t1;
    output->m[2].rtyp = RING_CMD; output->m[2].data = currRing;
    output->m[3].rtyp = MODUL_CMD; output->m[3].data = idCopy(M2);

    // Clean up
    id_Delete(&M2, RB);
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
    // Deserialize the output to verify


    // Clean up the copy after serialization

    return out_filename;
}



// Helper function to print an fq_nmod_mat_t matrix for debugging
void print_fq_nmod_mat(const char* label, const fq_nmod_mat_t mat, const fq_nmod_ctx_t ctx) {
    std::cout << label << "\n-------------\n";
    for (slong i = 0; i < fq_nmod_mat_nrows(mat, ctx); i++) {
        for (slong j = 0; j < fq_nmod_mat_ncols(mat, ctx); j++) {
            fq_nmod_print_pretty(fq_nmod_mat_entry(mat, i, j), ctx);
            std::cout << "  ";
        }
        std::cout << "\n";
    }
    std::cout << "-------------\n";
}

// Helper function to set an fq_nmod_mat_t to the identity matrix

// Manually set a matrix to the identity matrix
static void fq_nmod_mat_set_one(fq_nmod_mat_t mat, const fq_nmod_ctx_t ctx) {
    fq_nmod_mat_zero(mat, ctx);
    slong n = fq_nmod_mat_nrows(mat, ctx);
    slong m = fq_nmod_mat_ncols(mat, ctx);
    slong min = (n < m) ? n : m;
    fq_nmod_t one;
    fq_nmod_init(one, ctx);
    fq_nmod_one(one, ctx);
    for (slong i = 0; i < min; i++) {
        fq_nmod_mat_entry_set(mat, i, i, one, ctx);
    }
    fq_nmod_clear(one, ctx);
}

// Structure to hold Gauss reduction result
typedef struct {
    fq_nmod_mat_t P;  // Permutation matrix
    fq_nmod_mat_t U;  // Upper triangular matrix
    fq_nmod_mat_t S;  // Row echelon form of A
    slong rank;       // Rank of the matrix
} gaussred_result_t;

// Clear the gaussred result structure
static void gaussred_result_clear(gaussred_result_t* result, const fq_nmod_ctx_t ctx) {
    if (result) {
        fq_nmod_mat_clear(result->P, ctx);
        fq_nmod_mat_clear(result->U, ctx);
        fq_nmod_mat_clear(result->S, ctx);
    }
}
// Helper function to get unsigned integer value from fq_nmod element
static mp_limb_t fq_nmod_get_ui(const fq_nmod_t x, const fq_nmod_ctx_t ctx) {
    (void)ctx;  // Mark ctx as used to avoid warning
    return x->coeffs[0];  // For degree 1 fields, the value is in the first coefficient
}

// Helper function to convert SINGULAR matrix to FLINT fq_nmod_mat_t
int singular_to_fq_nmod_mat(fq_nmod_mat_t A, matrix m, ring R, const fq_nmod_ctx_t ctx) {
    slong r = m->rows(), c = m->cols();
    fq_nmod_mat_init(A, r, c, ctx);
    for (int i = 1; i <= r; i++) {
        for (int j = 1; j <= c; j++) {
            poly h = MATELEM(m, i, j);
            if (h != NULL) {
                if (p_Totaldegree(h, R) == 0) {
                    number coeff = pGetCoeff(h);
                    mp_limb_t val = n_Int(coeff, R->cf);
                    fq_nmod_t entry;
                    fq_nmod_init(entry, ctx);
                    fq_nmod_set_ui(entry, val, ctx);
                    fq_nmod_mat_entry_set(A, i-1, j-1, entry, ctx);
                    fq_nmod_clear(entry, ctx);
                } else {
                    WerrorS("Matrix entry is not constant");
                    fq_nmod_mat_clear(A, ctx);
                    return 0;
                }
            }
        }
    }
    return 1;
}

// Helper function to convert FLINT fq_nmod_mat_t to SINGULAR matrix
matrix fq_nmod_mat_to_singular(const fq_nmod_mat_t mat, ring R, const fq_nmod_ctx_t ctx) {
    slong r = fq_nmod_mat_nrows(mat, ctx), c = fq_nmod_mat_ncols(mat, ctx);
    matrix M = mpNew(r, c);
    for (slong i = 0; i < r; i++) {
        for (slong j = 0; j < c; j++) {
            fq_nmod_t entry;
            fq_nmod_init(entry, ctx);
            fq_nmod_set(entry, fq_nmod_mat_entry(mat, i, j), ctx);
            if (!fq_nmod_is_zero(entry, ctx)) {
                mp_limb_t val = fq_nmod_get_ui(entry, ctx);
                number n = n_Init(val, R->cf);
                MATELEM(M, i+1, j+1) = p_NSet(n, R);
            }
            fq_nmod_clear(entry, ctx);
        }
    }
    return M;
}

static gaussred_result_t gaussred_fq_nmod(const fq_nmod_mat_t A, const fq_nmod_ctx_t ctx);

static void cleanupList(lists L) {
    if (!L) return;
    for (int i = 0; i <= L->nr; i++) {
        if (L->m[i].data) {
            if (L->m[i].rtyp == MATRIX_CMD) {
                mp_Delete((matrix*)&L->m[i].data, currRing);
            }
        }
    }
}

// Main function to compute RREF using FLINT
lists singflint_rref_full(matrix m, ring R, int p) {
    if (!m || !R) {
        WerrorS("NULL matrix or ring");
        return NULL;
    }

    //int r = m->rows(), c = m->cols();
    //std::cout << "Rows: " << r << ", Cols: " << c << ", Ring: " << rString(R) << ", Coeff: " << nCoeffString(R->cf) << ", Char: " << rChar(R) << std::endl;

    ring savedRing = currRing;
    if (savedRing != R) rChangeCurrRing(R);

    // Initialize FLINT context
    fmpz_t pp;
    fmpz_init_set_ui(pp, p);
    fq_nmod_ctx_t ctx;
    fq_nmod_ctx_init(ctx, pp, 1, "x");

    // Convert SINGULAR matrix to FLINT
    fq_nmod_mat_t A;
    if (!singular_to_fq_nmod_mat(A, m, R, ctx)) {
        fq_nmod_ctx_clear(ctx);
        fmpz_clear(pp);
        if (savedRing != R) rChangeCurrRing(savedRing);
        return NULL;
    }

    // Compute RREF
    gaussred_result_t result = gaussred_fq_nmod(A, ctx);
   // std::cout << "Rank: " << result.rank << std::endl;

    // Convert results back to SINGULAR
    matrix P = fq_nmod_mat_to_singular(result.P, R, ctx);
    matrix U = fq_nmod_mat_to_singular(result.U, R, ctx);
    matrix M = fq_nmod_mat_to_singular(result.S, R, ctx);


    // Clean up FLINT objects
    gaussred_result_clear(&result, ctx);
    fq_nmod_mat_clear(A, ctx);
    fq_nmod_ctx_clear(ctx);
    fmpz_clear(pp);

    // Create result list
    lists L = (lists)omAlloc0Bin(slists_bin);
    L->Init(4);
    L->m[0].rtyp = MATRIX_CMD;
    L->m[0].data = P;
    L->m[1].rtyp = MATRIX_CMD;
    L->m[1].data = U;
    L->m[2].rtyp = MATRIX_CMD;
    L->m[2].data = M;
    L->m[3].rtyp = INT_CMD;
    L->m[3].data = (void*)(long)result.rank;

    if (savedRing != R) rChangeCurrRing(savedRing);
    return L;
}

// Wrapper function for Gauss reduction
lists singflintGaussRed(leftv args) {
    lists input_result = (lists)args->Data();
    lists result = (lists)input_result->m[3].Data();
    if (!result || result->nr < 2) {
        WerrorS("Invalid result list structure");
        return NULL;
    }

    matrix A = (matrix)result->m[0].Data();
    ring RZ = (ring)result->m[1].Data();
    int p = (int)(long)result->m[2].Data();

    ring savedRing = currRing;
    rChangeCurrRing(RZ);

    // Compute RREF
    lists Z = singflint_rref_full(A, RZ, p);
    if (!Z) {
        rChangeCurrRing(savedRing);
        return NULL;
    }

/*     matrix P = (matrix)Z->m[0].Data();
    matrix U = (matrix)Z->m[1].Data();
    matrix M = (matrix)Z->m[2].Data();
    int rank = (int)(long)Z->m[3].Data(); */

    // Debug output
   /*  std::cout << "P size: " << MATROWS(P) << "x" << MATCOLS(P) << std::endl;
    std::cout << "U size: " << MATROWS(U) << "x" << MATCOLS(U) << std::endl;
    std::cout << "M size: " << MATROWS(M) << "x" << MATCOLS(M) << std::endl;
    std::cout << "Rank: " << rank << std::endl;
    std::cout << "Current ring: " << rString(currRing) << ", RZ: " << rString(RZ) << std::endl;
 */
    // Create output list
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(4);

    lists t1 = (lists)omAlloc0Bin(slists_bin);
    t1->Init(2);
    t1->m[0].rtyp = STRING_CMD;
    t1->m[0].data = omStrDup("generators");
    t1->m[1].rtyp = STRING_CMD;
    t1->m[1].data = omStrDup("singflint_rref");

    output->m[0].rtyp = RING_CMD;
    output->m[0].data = currRing;
    output->m[1].rtyp = LIST_CMD;
    output->m[1].data = t1;
    output->m[2].rtyp = RING_CMD;
    output->m[2].data = currRing;
    output->m[3].rtyp = LIST_CMD;
    output->m[3].data = lCopy(Z);

    // Clean up
    cleanupList(Z);
    omFreeBin(Z, slists_bin);
    rChangeCurrRing(savedRing);

    return output;
}
// Implementation of performGaussRed function
lists performGaussRed(leftv args) {
    // This function is a wrapper around singflintGaussRed
    return singflintGaussRed(args);
}

// Gaussian elimination with partial pivoting over Fq_nmod
static gaussred_result_t gaussred_fq_nmod(const fq_nmod_mat_t A, const fq_nmod_ctx_t ctx) {
    slong n = fq_nmod_mat_nrows(A, ctx);
    slong m = fq_nmod_mat_ncols(A, ctx);
    slong mr = (n < m) ? n : m;  // Minimum of rows and columns
    slong rank = 0;

    // Initialize result structure
    gaussred_result_t result;
    fq_nmod_mat_init(result.P, n, n, ctx);
    fq_nmod_mat_init(result.U, n, n, ctx);
    fq_nmod_mat_init(result.S, n, m, ctx);
    fq_nmod_mat_set(result.S, A, ctx);  // S starts as a copy of A

    // Initialize P and U as identity matrices
    fq_nmod_mat_set_one(result.P, ctx);
    fq_nmod_mat_set_one(result.U, ctx);

    fq_nmod_t max_coeff, pivot, c, temp, zero, one;
    fq_nmod_init(max_coeff, ctx);
    fq_nmod_init(pivot, ctx);
    fq_nmod_init(c, ctx);
    fq_nmod_init(temp, ctx);
    fq_nmod_init(zero, ctx);
    fq_nmod_init(one, ctx);
    fq_nmod_zero(zero, ctx);
    fq_nmod_one(one, ctx);

    slong col = 0;
    for (slong i = 0; i < mr && col < m; i++) {
        // Find pivot
        slong jp = i;
        fq_nmod_set(max_coeff, fq_nmod_mat_entry(result.S, i, col), ctx);
        for (slong j = i + 1; j < n; j++) {
            if (!fq_nmod_is_zero(fq_nmod_mat_entry(result.S, j, col), ctx)) {
                jp = j;
                fq_nmod_set(max_coeff, fq_nmod_mat_entry(result.S, j, col), ctx);
                break;
            }
        }

        if (fq_nmod_is_zero(max_coeff, ctx)) {
            col++;
            i--;
            continue;
        }

        // Swap rows if necessary
        if (jp != i) {
            fq_nmod_mat_swap_rows(result.S, NULL, i, jp, ctx);
            fq_nmod_mat_swap_rows(result.P, NULL, i, jp, ctx);
            //std::cout << "[DEBUG] Swapped row " << i << " with row " << jp << std::endl;
        }

        // Set pivot
        fq_nmod_set(pivot, fq_nmod_mat_entry(result.S, i, col), ctx);
        //std::cout << "[DEBUG] Row " << i << " reduced, pivot in column " << col << ": ";
        //fq_nmod_print_pretty(pivot, ctx);
        //std::cout << std::endl;

        // Normalize the pivot row - this is the critical fix
        if (!fq_nmod_is_one(pivot, ctx)) {
            for (slong k = col; k < m; k++) {
                fq_nmod_div(fq_nmod_mat_entry(result.S, i, k), 
                           fq_nmod_mat_entry(result.S, i, k), 
                           pivot, ctx);
            }
            // Update U matrix to reflect normalization
            fq_nmod_set(fq_nmod_mat_entry(result.U, i, i), pivot, ctx);
            // Set pivot to 1 after normalization
            fq_nmod_set(pivot, one, ctx);
            fq_nmod_set(fq_nmod_mat_entry(result.S, i, col), one, ctx);
            //std::cout << "[DEBUG] Normalized pivot row " << i << std::endl;
        }

        // Eliminate below pivot
        for (slong j = i + 1; j < n; j++) {
            if (!fq_nmod_is_zero(fq_nmod_mat_entry(result.S, j, col), ctx)) {
                fq_nmod_div(c, fq_nmod_mat_entry(result.S, j, col), pivot, ctx);
                for (slong k = col + 1; k < m; k++) {
                    fq_nmod_mul(temp, c, fq_nmod_mat_entry(result.S, i, k), ctx);
                    fq_nmod_sub(fq_nmod_mat_entry(result.S, j, k), 
                               fq_nmod_mat_entry(result.S, j, k), temp, ctx);
                }
                fq_nmod_set(fq_nmod_mat_entry(result.U, j, i), c, ctx);
                fq_nmod_set(fq_nmod_mat_entry(result.S, j, col), zero, ctx);
            }
        }

        rank++;
        col++;
    }

    result.rank = rank;

    fq_nmod_clear(max_coeff, ctx);
    fq_nmod_clear(pivot, ctx);
    fq_nmod_clear(c, ctx);
    fq_nmod_clear(temp, ctx);
    fq_nmod_clear(zero, ctx);
    fq_nmod_clear(one, ctx);

    return result;
}


// Add type defin
// Forward declarations
lists flint_getSortedIntegrals_wrapper(leftv args);

// Helper function to serialize a list to a string for hashing
static std::string serializeList(lists L) {
    if (!L) return "";
    std::string s;
    //std::cout << "DEBUG: Serializing list of size " << (L->nr + 1) << std::endl;
    for (int i = 0; i <= L->nr; i++) {
        if (L->m[i].rtyp == INT_CMD) {
            int val = (int)(long)L->m[i].data;
            s += std::to_string(val) + ",";
           // std::cout << "DEBUG: Added value " << val << " to serialization" << std::endl;
        } else {
            std::cout << "DEBUG: Skipping non-integer type " << L->m[i].rtyp << " at index " << i << std::endl;
        }
    }
    //std::cout << "DEBUG: Final serialization: " << s << std::endl;
    return s;
}

// Helper function to convert a list to a Singular-style string
static std::string listToString(lists L) {
    if (!L) return "NULL";
    std::string s;
    for (int i = 0; i <= L->nr; i++) {
        if (L->m[i].rtyp == INT_CMD) {
            s += std::to_string((int)(long)L->m[i].data);
        } else if (L->m[i].rtyp == LIST_CMD) {
            s += listToString((lists)L->m[i].data);
        } else if (L->m[i].rtyp == POLY_CMD) {
            poly p = (poly)L->m[i].data;
            if (p) {
                char* str = p_String(p, currRing);
                s += str;
                omFree(str);
            } else {
                s += "0";
            }
        } else if (L->m[i].rtyp == NUMBER_CMD) {
            number n = (number)L->m[i].data;
            if (n) {
                StringSetS("");
                n_Write(n, currRing->cf);
                char* str = StringEndS();
                s += str;
                omFree(str);
            } else {
                s += "0";
            }
        } else {
            s += "?";
        }
        if (i < L->nr) s += ",";
    }
    return s;
}

// Helper function to compute sort measures (equivalent to Singular's getSortMeasures)
static lists getSortMeasures(lists ind) {
    if (!ind) return NULL;

    // Initialize variables
    int Nprop = 0;  // Number of positive indices
    int Nid = 0;    // Binary encoding of positive indices
    int r = 0;      // Sum of positive indices
    int s = 0;      // Sum of absolute values of non-positive indices
    std::vector<int> rv;  // Positive indices
    std::vector<int> sv;  // Non-positive indices

    // Process each index
    for (int j = 0; j <= ind->nr; j++) {
        if (ind->m[j].rtyp != INT_CMD) continue;
        int val = (int)(long)ind->m[j].data;

        if (val > 0) {
            Nprop++;
            Nid += (1 << j);  // 2^j
            r += val;
            rv.push_back(val);
        } else {
            s += abs(val);  // Don't add 1 to match Singular version
            sv.push_back(val);
        }
    }

    // Create result list: [Nprop, Nid, r, s] + original indices
    lists result = (lists)omAlloc0Bin(slists_bin);
    if (!result) return NULL;

    int total_size = 4 + ind->nr + 1;
    result->Init(total_size);
    result->nr = total_size - 1;

    // Populate [Nprop, Nid, r, s]
    result->m[0].rtyp = INT_CMD;
    result->m[0].data = (void*)(long)Nprop;
    result->m[1].rtyp = INT_CMD;
    result->m[1].data = (void*)(long)Nid;
    result->m[2].rtyp = INT_CMD;
    result->m[2].data = (void*)(long)r;
    result->m[3].rtyp = INT_CMD;
    result->m[3].data = (void*)(long)s;  // Don't add 1 to match Singular version

    // Add original indices in order
    for (int i = 0; i <= ind->nr; i++) {
        if (ind->m[i].rtyp == INT_CMD) {
            result->m[i + 4].rtyp = INT_CMD;
            result->m[i + 4].data = (void*)(long)((int)(long)ind->m[i].data);
        }
    }

    return result;
}

// Helper function to compare lists lexicographically
static int compareListsLex(lists L1, lists L2) {
    if (!L1 || !L2) {
        std::cout << "DEBUG: compareListsLex - One or both lists are NULL" << std::endl;
        return L1 == L2 ? 0 : (L1 ? 1 : -1);
    }
    
    std::cout << "DEBUG: Comparing lists:" << std::endl;
    std::cout << "L1: " << listToString(L1) << std::endl;
    std::cout << "L2: " << listToString(L2) << std::endl;
    
    // First compare the first 4 elements (Nprop, Nid, r, s)
    for (int i = 0; i < 4; i++) {
        if (L1->m[i].rtyp != L2->m[i].rtyp) {
            std::cout << "DEBUG: Type mismatch at index " << i << ": L1=" << L1->m[i].rtyp << ", L2=" << L2->m[i].rtyp << std::endl;
            return L1->m[i].rtyp < L2->m[i].rtyp ? -1 : 1;
        }
        if (L1->m[i].rtyp == INT_CMD) {
            int v1 = (int)(long)L1->m[i].data;
            int v2 = (int)(long)L2->m[i].data;
            if (v1 != v2) {
                std::cout << "DEBUG: Value mismatch at index " << i << ": L1=" << v1 << ", L2=" << v2 << std::endl;
                return v1 < v2 ? -1 : 1;
            }
        }
    }
    
    // Then compare the remaining elements in reverse order
    int min_size = std::min(L1->nr, L2->nr);
    for (int i = min_size; i >= 4; i--) {
        if (L1->m[i].rtyp != L2->m[i].rtyp) {
            std::cout << "DEBUG: Type mismatch at index " << i << ": L1=" << L1->m[i].rtyp << ", L2=" << L2->m[i].rtyp << std::endl;
            return L1->m[i].rtyp < L2->m[i].rtyp ? -1 : 1;
        }
        if (L1->m[i].rtyp == INT_CMD) {
            int v1 = (int)(long)L1->m[i].data;
            int v2 = (int)(long)L2->m[i].data;
            if (v1 != v2) {
                std::cout << "DEBUG: Value mismatch at index " << i << ": L1=" << v1 << ", L2=" << v2 << std::endl;
                return v1 < v2 ? -1 : 1;
            }
        }
    }
    
    int result = L1->nr < L2->nr ? -1 : (L1->nr > L2->nr ? 1 : 0);
    std::cout << "DEBUG: Final comparison result: " << result << std::endl;
    return result;
}

// Implementation of lexSort for lists
lists lexSort(lists L) {
    if (!L || L->nr < 0) return L;

    // Create array of indices for sorting
    std::vector<int> indices(L->nr + 1);
    for (int i = 0; i <= L->nr; i++) {
        indices[i] = i;
    }

    // Sort indices based on list elements using bubble sort
    for (int i = 0; i < L->nr; i++) {
        for (int j = 0; j < L->nr - i; j++) {
            if (L->m[j].rtyp != LIST_CMD || L->m[j+1].rtyp != LIST_CMD) continue;
            lists Lj = (lists)L->m[j].data;
            lists Lj1 = (lists)L->m[j+1].data;
            if (!Lj || !Lj1) continue;
            
            // Compare sort measures
            if (compareListsLex(Lj, Lj1) > 0) {
                // Swap elements
                sleftv temp = L->m[j];
                L->m[j] = L->m[j+1];
                L->m[j+1] = temp;
            }
        }
    }

    return L;
}


lists flint_getSortedIntegrals(setIBP S) {
    if (!S) {
        WerrorS("NULL S in flint_getSortedIntegrals");
        return NULL;
    }
    if (!S->IBP) {
        WerrorS("NULL S->IBP in flint_getSortedIntegrals");
        return NULL;
    }

    // Count total number of indices and debug IBP structure
    int total_indices = 0;
    int ibp_count = 0;
    while (S->IBP[ibp_count] != NULL) {
        if (S->IBP[ibp_count]->i) {
            total_indices += S->IBP[ibp_count]->i->nr + 1;
        }
        ibp_count++;
    }

    // Initialize pairs list with more precise capacity
    int capacity = std::max(16, total_indices / 4);
    lists pairs = (lists)omAlloc0Bin(slists_bin);
    if (!pairs) {
        WerrorS("Memory allocation failed for pairs");
        return NULL;
    }
    pairs->Init(capacity);
    pairs->nr = -1;

    // Use hash set for duplicate checking with more efficient memory usage
    std::unordered_set<std::string> seen_indices;
    seen_indices.reserve(capacity);

    // Process each IBP relation
    for (int j = 0; j < ibp_count; j++) {
        oneIBP I = S->IBP[j];
        if (!I || !I->i) continue;

        for (int k = 0; k <= I->i->nr; k++) {
            if (!I->i->m[k].data || I->i->m[k].rtyp != LIST_CMD) continue;

            lists ind = (lists)I->i->m[k].data;
            std::string serial = serializeList(ind);

            // Check for duplicates
            if (seen_indices.find(serial) == seen_indices.end()) {
                seen_indices.insert(serial);

                // Compute sort measures
                lists measures = getSortMeasures(ind);
                if (!measures) {
                    // Clean up before returning
                    omFreeBin(pairs, slists_bin);
                    WerrorS("Failed to compute sort measures");
                    return NULL;
                }

                if (pairs->nr + 1 >= capacity) {
                    int new_capacity = capacity * 2;
                    lists new_pairs = (lists)omAlloc0Bin(slists_bin);
                    if (!new_pairs) {
                        // Clean up before returning
                        omFreeBin(pairs, slists_bin);
                        WerrorS("Memory allocation failed when expanding pairs");
                        return NULL;
                    }
                    new_pairs->Init(new_capacity);
                    for (int i = 0; i <= pairs->nr; i++) {
                        new_pairs->m[i].rtyp = pairs->m[i].rtyp;
                        new_pairs->m[i].data = pairs->m[i].data;
                    }
                    new_pairs->nr = pairs->nr;
                    omFreeBin(pairs, slists_bin);
                    pairs = new_pairs;
                    capacity = new_capacity;
                }

                // Add pair (ind, measures)
                pairs->nr++;
                lists pair = (lists)omAlloc0Bin(slists_bin);
                if (!pair) {
                    // Clean up before returning
                    omFreeBin(pairs, slists_bin);
                    WerrorS("Memory allocation failed for pair");
                    return NULL;
                }
                pair->Init(2);
                pair->nr = 1;
                pair->m[0].rtyp = LIST_CMD;
                pair->m[0].data = lCopy(ind);
                pair->m[1].rtyp = LIST_CMD;
                pair->m[1].data = measures;
                pairs->m[pairs->nr].rtyp = LIST_CMD;
                pairs->m[pairs->nr].data = pair;
            }
        }
    }

    // Create final result list
    lists result = (lists)omAlloc0Bin(slists_bin);
    if (!result) {
        // Clean up before returning
        omFreeBin(pairs, slists_bin);
        WerrorS("Memory allocation failed for result list");
        return NULL;
    }
    result->Init(pairs->nr + 1);
    result->nr = -1;

    // Sort pairs by sort measures
    if (pairs->nr > 0) {
        try {
            std::vector<lists> sorted_pairs(pairs->nr + 1);
            for (int i = 0; i <= pairs->nr; i++) {
                sorted_pairs[i] = (lists)pairs->m[i].data;
            }

            // Sort with more detailed comparison
            std::sort(sorted_pairs.begin(), sorted_pairs.end(), [](lists a, lists b) {
                if (!a || !b || !a->m[1].data || !b->m[1].data) return false;
                
                // First compare the first 4 elements (Nprop, Nid, r, s)
                lists measures_a = (lists)a->m[1].data;
                lists measures_b = (lists)b->m[1].data;
                
                for (int i = 0; i < 4; i++) {
                    if (measures_a->m[i].rtyp != measures_b->m[i].rtyp) {
                        return measures_a->m[i].rtyp < measures_b->m[i].rtyp;
                    }
                    if (measures_a->m[i].rtyp == INT_CMD) {
                        int v1 = (int)(long)measures_a->m[i].data;
                        int v2 = (int)(long)measures_b->m[i].data;
                        if (v1 != v2) return v1 < v2;
                    }
                }
                
                // If measures are equal, compare the indices
                lists ind_a = (lists)a->m[0].data;
                lists ind_b = (lists)b->m[0].data;
                
                int min_size = std::min(ind_a->nr, ind_b->nr);
                for (int i = 0; i <= min_size; i++) {
                    if (ind_a->m[i].rtyp != ind_b->m[i].rtyp) {
                        return ind_a->m[i].rtyp < ind_b->m[i].rtyp;
                    }
                    if (ind_a->m[i].rtyp == INT_CMD) {
                        int v1 = (int)(long)ind_a->m[i].data;
                        int v2 = (int)(long)ind_b->m[i].data;
                        if (v1 != v2) return v1 < v2;
                    }
                }
                
                return ind_a->nr < ind_b->nr;
            });

            // Add sorted pairs to result
            for (int i = 0; i <= pairs->nr; i++) {
                if (!sorted_pairs[i]) continue;
                lists pair = sorted_pairs[i];
                result->nr++;
                result->m[result->nr].rtyp = LIST_CMD;
                result->m[result->nr].data = pair;
            }
        } catch (...) {
            // Clean up on exception
            omFreeBin(pairs, slists_bin);
            omFreeBin(result, slists_bin);
            WerrorS("Exception during sorting");
            return NULL;
        }
    }

    // Clean up temporary data
    omFreeBin(pairs, slists_bin);
    return result;
}

lists flint_getSortedIntegrals_wrapper(leftv args) {
    lists input = (lists)args->Data();
    if (!input || input->m[3].rtyp != LIST_CMD || !input->m[3].data) {
        WerrorS("Invalid input in flint_getSortedIntegrals_wrapper");
        return NULL;
    }

    lists SS = (lists)input->m[3].data;
    ring R = (ring)SS->m[0].data;
    lists ibp = (lists)SS->m[3].data;
    if (!ibp) {
        WerrorS("Invalid IBP data");
        return NULL;
    }

  /*  // Debug input structure
   // std::cout << "DEBUG: Input IBP list size: " << (ibp->nr + 1) << std::endl;
    for (int i = 0; i <= ibp->nr; i++) {
        if (ibp->m[i].rtyp == LIST_CMD && ibp->m[i].data) {
            lists ibp_item = (lists)ibp->m[i].data;
            if (ibp_item->nr >= 1 && ibp_item->m[1].rtyp == LIST_CMD) {
               // std::cout << "DEBUG: IBP[" << (i + 1) << "] seeds: " << listToString((lists)ibp_item->m[1].data) << std::endl;
            }
        }
    } */

    // Create setIBP structure
    setIBP S = (setIBP)omAlloc0(sizeof(*S));
   
    
    S->IBP = (oneIBP*)omAlloc0(sizeof(oneIBP) * (ibp->nr + 2));
   
    
    // Copy IBP data
    for (int i = 0; i <= ibp->nr; i++) {
        if (ibp->m[i].rtyp == LIST_CMD && ibp->m[i].data) {
            lists ibp_item = (lists)ibp->m[i].data;
            S->IBP[i] = (oneIBP)omAlloc0(sizeof(*S->IBP[i]));
            if (!S->IBP[i]) {
                WerrorS("Memory allocation failed for IBP item");
                for (int j = 0; j < i; j++) {
                    if (S->IBP[j]) omFreeSize(S->IBP[j], sizeof(*S->IBP[j]));
                }
                omFreeSize(S->IBP, sizeof(oneIBP) * (ibp->nr + 2));
                omFreeSize(S, sizeof(*S));
        return NULL;
    }
            if (ibp_item->nr >= 1) {
                S->IBP[i]->i = (lists)ibp_item->m[1].data;
                S->IBP[i]->c = (lists)ibp_item->m[0].data;
            }
        }
    }
    S->IBP[ibp->nr + 1] = NULL; // Ensure NULL terminator
    S->over = R;
std::cout<<"first element of S.IBP: "<<listToString((lists)S->IBP[0]->i)<<std::endl;
/* std::cout<<"first element of S.IBP->c: "<<listToString((lists)S->IBP[0]->c)<<std::endl;
std::cout<<"first element of S.IBP->i: "<<listToString((lists)S->IBP[0]->i)<<std::endl;
std::cout<<"size of S.IBP: "<<ibp->nr+1<<std::endl; */
    // Call main function
    lists result = flint_getSortedIntegrals(S);
   /*  lists result1=(lists)result->m[0].data;
    std::cout<<"result1 of size="<<result1->nr+1<<std::endl;
    std::cout<<"result1="<<listToString(result1)<<std::endl;
    lists result1_0=(lists)result1->m[0].data;
    lists result1_1=(lists)result1->m[1].data;
    std::cout<<"result1_0 of size="<<result1_0->nr+1<<std::endl;
    std::cout<<"result1_0="<<listToString(result1_0)<<std::endl;
    std::cout<<"result1_1 of size="<<result1_1->nr+1<<std::endl;
    std::cout<<"result1_1="<<listToString(result1_1)<<std::endl;

    lists result2=(lists)result->m[1].data;
    std::cout<<"result2 of size="<<result2->nr+1<<std::endl;
    std::cout<<"result2="<<listToString(result2)<<std::endl;
    lists result3=(lists)result->m[2].data;
    std::cout<<"result3 of size="<<result3->nr+1<<std::endl;
    std::cout<<"result3="<<listToString(result3)<<std::endl; */
    //std::cout << "DEBUG: flint_getSortedIntegrals completed" << std::endl;

//PRINTING END OF RESULT
//lists end_result=(lists)result->m[43].data;
//std::cout<<"end_result of size="<<end_result->nr+1<<std::endl;
//std::cout<<"end_result="<<listToString(end_result)<<std::endl;

// Create output list
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(4);

    lists t1 = (lists)omAlloc0Bin(slists_bin);
    t1->Init(2);
    t1->m[0].rtyp = STRING_CMD;
    t1->m[0].data = omStrDup("generators");
    t1->m[1].rtyp = STRING_CMD;
    t1->m[1].data = omStrDup("getSortedIntegrals");

    output->m[0].rtyp = RING_CMD;
    output->m[0].data = currRing;
    output->m[1].rtyp = LIST_CMD;
    output->m[1].data = t1;
    output->m[2].rtyp = RING_CMD;
    output->m[2].data = currRing;
    output->m[3].rtyp = LIST_CMD;
    output->m[3].data = lCopy(result);

    // Clean up
    for (int i = 0; i <= ibp->nr; i++) {
        if (S->IBP[i]) {
            omFreeSize(S->IBP[i], sizeof(*S->IBP[i]));
        }
    }
    omFreeSize(S->IBP, sizeof(oneIBP) * (ibp->nr + 2));
    omFreeSize(S, sizeof(*S));

    if (!result) {
        result = (lists)omAlloc0Bin(slists_bin);
        result->Init(0);
    }

    return output;
}

std::string singular_getSortedIntegrals(std::string const& res,
    std::string const& needed_library,
    std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);
    std::pair<int, lists> Res;
    std::string ids;
    std::string out_filename;
    
    ids = worker();
    Res = deserialize(res, ids);
    
    ScopedLeftv args(Res.first, lCopy(Res.second));
    
    lists result = flint_getSortedIntegrals_wrapper(args.leftV());
    std::cout<<"sorted integrals="<<listToString(result)<<std::endl;
    lists result1=(lists)result->m[3].data;
    std::cout<<"sorted integrals of size="<<result1->nr+1<<std::endl;
   
    out_filename = serialize(result, base_filename);
    
    return out_filename;
}

// Substitute parameters in a polynomial (substituteList)
poly substituteList(poly f, lists l, ring R) {
    if (f == NULL) return NULL;
   
    
    poly result = p_Copy(f, R);
   // std::cout<<"poly result before substitution="<<pString(result)<<std::endl;
    

    // For each parameter in the list
    for (int i = 0; i <= l->nr; i++) {
        
        // Create a polynomial from the integer value
        poly val_poly = p_ISet((int)(long)l->m[i].data, R);
        
        // Substitute the parameter - parameters are 1-based indexed
        result = pSubstPar(result, i+1, val_poly);
        
        // Clean up
        p_Delete(&val_poly, R);
    }
    
    return result;
}

lists getRandom(int p, int s) {
    if (p <= 0 || s <= 0) {
        WerrorS("p and s must be positive integers");
        return NULL;
    }
    
    lists L = (lists)omAlloc0Bin(slists_bin);
    if (!L) {
        WerrorS("Memory allocation failed for output list");
        return NULL;
    }
    L->Init(s);
    
    flint_rand_t state;
    fmpz_t m;
    flint_randinit(state);
    fmpz_init_set_ui(m, p);
    
    // Generate random integers between -(p-1) and (p-1)
    for (int i = 0; i < s; i++) {
        // Generate a random number between 0 and 2*(p-1)
        fmpz_t val;
        fmpz_init(val);
        fmpz_randm(val, state, m);
        fmpz_mul_ui(val, val, 2);
        
        // Subtract (p-1) to get range from -(p-1) to (p-1)
        fmpz_sub_ui(val, val, p-1);
        L->m[i].rtyp = INT_CMD;
        L->m[i].data = (void*)(long)fmpz_get_si(val);
        
        // Clean up FLINT integer
        fmpz_clear(val);
    }
    
    // Clean up
    fmpz_clear(m);
    flint_randclear(state);
    
    return L;
}
// Extract coefficients (extractCoef)


// Structure to hold FLINT polynomial coefficients
struct FLINTPolyCoeff {
    fmpq_poly_t poly;
    int var_index;  // Variable index this coefficient belongs to
};

// Structure for IBP relation using FLINT
struct FLINT_IBP {
    std::vector<std::vector<int>> indices;  // List of indices
    std::vector<FLINTPolyCoeff> coefficients;  // Polynomial coefficients
};

// Forward declarations
static void convertSingularToFLINT(fmpq_t result, number n, ring R);
static number convertFLINTToSingular(const fmpq_t q, ring R);

// Helper function to convert Singular number to FLINT rational
static inline void convertSingularToFLINT(fmpq_t result, number n, ring R) {
    if (n_IsZero(n, R->cf)) {
        fmpq_zero(result);
    } else {
        // For rational numbers, get numerator and denominator
        number num = n_GetNumerator(n, R->cf);
        number den = n_GetDenom(n, R->cf);
        
        // Convert to FLINT format
        fmpz_set_si(fmpq_numref(result), n_Int(num, R->cf));
        fmpz_set_si(fmpq_denref(result), n_Int(den, R->cf));
        
        // Clean up
        n_Delete(&num, R->cf);
        n_Delete(&den, R->cf);
    }
}

// Helper function to convert FLINT rational to Singular number
static inline number convertFLINTToSingular(const fmpq_t q, ring R) {
    if (fmpq_is_zero(q)) {
        return n_Init(0, R->cf);
    } else {
        // Get numerator and denominator as integers
        long num = fmpz_get_si(fmpq_numref(q));
        long den = fmpz_get_si(fmpq_denref(q));
        
        // Create Singular number
        number n = n_Init(num, R->cf);
        if (den != 1) {
            number d = n_Init(den, R->cf);
            number result = n_Div(n, d, R->cf);
            n_Delete(&n, R->cf);
            n_Delete(&d, R->cf);
            return result;
        }
        return n;
    }
}

// Implementation of extractCoef
// USAGE:   extractCoef(I,ind,l); I oneIBP,ind list,l list,
// ASSUME:   ind is the output of getSortedIntegrals, and l is the list of values over the base field I.baikovover. 
//          size(l)=npars(I.baikovover)
// RETURN:   list of values where, the i-th element is the evaluation of coefficient function at values in the list l 
//          of the IBP relation oneIBP, whose index is i=ind[i][1].
static int comp_lex(lists l1, lists l2) {
    for (int i = 0; i <= l1->nr; i++) {
        if (l1->m[i].rtyp != INT_CMD || l2->m[i].rtyp != INT_CMD) continue;
        int v1 = (int)(long)l1->m[i].data;
        int v2 = (int)(long)l2->m[i].data;
        if (v1 < v2) return -1;
        if (v1 > v2) return 1;
    }
    return 0;
}

lists extractCoef(oneIBP I, lists ind, lists l, ring R) {
    if (!I || !ind || !l || !R) return NULL;
    
  /*   std::cout << "Entering extractCoef with:" << std::endl;
    std::cout << "I->i = " << listToString(I->i) << std::endl;
    std::cout << "I->c = " << listToString(I->c) << std::endl;
    std::cout << "ind = " << listToString(ind) << std::endl;
    std::cout << "l = " << listToString(l) << std::endl;
     */
    // Create result list with same size as ind
    lists result = (lists)omAlloc0Bin(slists_bin);
    result->Init(ind->nr + 1);
    
    // For each index in ind
    for (int j = 0; j <= ind->nr; j++) {
        // Get ind[j][1] - the first element of the target indices
        lists target_indices = (lists)ind->m[j].data;
        if (!target_indices) {
           // std::cout << "target_indices is NULL for j=" << j << std::endl;
            continue;
        }
        lists target = (lists)target_indices->m[0].data;
        if (!target) {
         //   std::cout << "target is NULL for j=" << j << std::endl;
            continue;
        }
        
        //std::cout << "Processing index j=" << j << ", target=" << listToString(target) << std::endl;
        
        // Search for matching indices in I->i
        bool found = false;
        for (int k = 0; k <= I->i->nr; k++) {
            lists ibp_indices = (lists)I->i->m[k].data;
            if (!ibp_indices) continue;
            
           // std::cout << "Comparing with I->i[" << k << "]=" << listToString(ibp_indices) << std::endl;
            
            if (comp_lex(target, ibp_indices) == 0) {
                found = true;
                //std::cout << "Found match at k=" << k << std::endl;
                
                // Found matching indices, get the coefficient
                if (I->c->m[k].rtyp == NUMBER_CMD) {
                    number n = (number)I->c->m[k].data;
                    //std::cout << "Original coefficient: ";
                    //printNumber(n, R);
                    //std::cout << std::endl;
                    
                    poly p = p_NSet(n_Copy(n, R->cf), R);
                    if (p) {
                        //std::cout << "Before substitution: " << p_String(p, R) << std::endl;
                        // Only substitute if we found a match
                        poly substituted = substituteList(p, l, R);
                        //std::cout << "After substitution: " << p_String(substituted, R) << std::endl;
                        
                        result->m[j].rtyp = NUMBER_CMD;  //  NUMBER_CMD
                        number coeff = p_GetCoeff(substituted, R);
                        result->m[j].data = n_Copy(coeff, R->cf);
                        p_Delete(&p, R);
                        p_Delete(&substituted, R);
                    }
                } else {
                    //std::cout << "Coefficient at k=" << k << " is not a number, type=" << I->c->m[k].rtyp << std::endl;
                }
                break;
            }
        }
        
        // If no match found, set coefficient to 0
        if (!found) {
            //std::cout << "No match found for j=" << j << std::endl;
            result->m[j].rtyp = NUMBER_CMD;
            result->m[j].data = n_Init(0, R->cf);
        }
    }
    
    //std::cout << "Final result of extractCoef=" << listToString(result) << std::endl;
    return result;
}
// Helper function to convert Singular poly to FLINT poly
void convertSingularPolyToFLINT(fmpq_poly_t flint_poly, poly p, ring R) {
    fmpq_poly_init(flint_poly);
    
    while (p != NULL) {
        // Get exponent vector
        int exp = p_GetExp(p, 1, R); // Assuming univariate for simplicity
        
        // Get coefficient
        number n = p_GetCoeff(p, R);
        fmpq_t coeff;
        fmpq_init(coeff);
        
        // Convert Singular number to FLINT rational
        convertSingularToFLINT(coeff, n, R);
        
        // Set coefficient in polynomial
        fmpq_poly_set_coeff_fmpq(flint_poly, exp, coeff);
        
        fmpq_clear(coeff);
        p = pNext(p);
    }
}


// Set matrix using FLINT (setMat)
matrix setMat(setIBP S, lists val, lists ind, ring R) {
    if (!S || !val || !ind || !R) return NULL;
    
    // Count the number of IBP relations and indices
    int num_ibp = 0;
    while (S->IBP[num_ibp] != NULL) num_ibp++;
    
    int num_indices = 0;
    while (ind->m[num_indices].rtyp == LIST_CMD && ind->m[num_indices].data) {
        num_indices++;
    }
    
    // Create result matrix
    matrix result = mpNew(num_ibp, num_indices);
    if (!result) {
        WerrorS("Memory allocation failed for result matrix");
        return NULL;
    }
    
    // For each IBP relation
    for (int i = 0; i < num_ibp; i++) {
        // Extract coefficients for this IBP
        lists coef = extractCoef(S->IBP[i], ind, val, R);
        if (!coef) {
            // Clean up on error
            idDelete((ideal*)&result);
            WerrorS("Failed to extract coefficients");
            return NULL;
        }
       
        // Fill the matrix row with coefficients
        for (int j = 0; j < num_indices; j++) {
            if (j <= coef->nr) {
                number n = (number)coef->m[j].data;
                if (n) {
                    MATELEM(result, i+1, j+1) = p_NSet(n_Copy(n, R->cf), R);
                }
            }
        }
        
        // Clean up coefficient list
        for (int j = 0; j <= coef->nr; j++) {
            if (coef->m[j].rtyp == NUMBER_CMD) {
                n_Delete((number*)&coef->m[j].data, R->cf);
            }
        }
        omFreeBin(coef, slists_bin);
    }
    
    return result;
}

lists setMat(leftv args) {
    std::cout << "[DEBUG] Entering setMat" << std::endl;
    
    if (!args || !args->Data()) {
        WerrorS("Invalid arguments");
        return NULL;
    }
    
    // Extract input data
    lists input_S = (lists)args->Data();
    if (!input_S || input_S->m[3].rtyp != LIST_CMD || !input_S->m[3].data) {
        WerrorS("Invalid input_S structure");
        return NULL;
    }
    
    lists Set = (lists)input_S->m[3].data;
    ring R = (ring)Set->m[0].data;
    std::cout << "[DEBUG] Got ring R: " << rString(R) << std::endl;
    
    if (!args->next || !args->next->Data()) {
        WerrorS("Missing input_ind argument");
        return NULL;
    }
    lists input_ind = (lists)(args->next->Data());
    
    int p = (int)(long)(args->next->next->Data());
    if (p <= 0) {
        WerrorS("Invalid prime number");
        return NULL;
    }
    //std::cout << "[DEBUG] Using prime p = " << p << std::endl;
    
    // Extract IBP and index lists
    lists ibp = (lists)Set->m[3].data;
    lists ind = (lists)input_ind->m[3].data;
    if (!ibp || !ind) {
        WerrorS("Invalid IBP or index list");
        return NULL;
    }
    //std::cout << "[DEBUG] Got IBP list size: " << ibp->nr + 1 << std::endl;
    //std::cout << "[DEBUG] Got index list size: " << ind->nr + 1 << std::endl;
    
    // Create setIBP structure
    setIBP S = (setIBP)omAlloc0(sizeof(*S));
    if (!S) {
        WerrorS("Memory allocation failed for setIBP");
        return NULL;
    }
    
    S->IBP = (oneIBP*)omAlloc0(sizeof(oneIBP) * (ibp->nr + 2));
    if (!S->IBP) {
        omFreeSize(S, sizeof(*S));
        WerrorS("Memory allocation failed for IBP array");
        return NULL;
    }
    
    // Copy IBP data
    int validIBPs = 0;
    for (int i = 0; i <= ibp->nr; i++) {
        if (ibp->m[i].rtyp == LIST_CMD && ibp->m[i].data) {
            lists ibp_item = (lists)ibp->m[i].data;
            S->IBP[validIBPs] = (oneIBP)omAlloc0(sizeof(*S->IBP[validIBPs]));
            if (!S->IBP[validIBPs]) {
                // Clean up on error
                for (int j = 0; j < validIBPs; j++) {
                    if (S->IBP[j]) omFreeSize(S->IBP[j], sizeof(*S->IBP[j]));
                }
                omFreeSize(S->IBP, sizeof(oneIBP) * (ibp->nr + 2));
                omFreeSize(S, sizeof(*S));
                WerrorS("Memory allocation failed for IBP item");
                return NULL;
            }
            
            if (ibp_item->nr >= 1) {
                S->IBP[validIBPs]->c = (lists)ibp_item->m[0].data;
                S->IBP[validIBPs]->i = (lists)ibp_item->m[1].data;
                validIBPs++;
            }
        }
    }
    S->IBP[validIBPs] = NULL;
    S->over = R;
    //std::cout << "[DEBUG] Created setIBP with " << validIBPs << " valid IBPs" << std::endl;
    
    // Generate random values for parameters
    lists input_val = getRandom(p, rPar(R));
    if (!input_val) {
        WerrorS("Failed to generate random values");
        return NULL;
    }
    //std::cout << "[DEBUG] Generated " << input_val->nr + 1 << " random values" << std::endl;
    
    // Compute matrix in ring R
    matrix result = setMat(S, input_val, ind, R);
    if (!result) {
        WerrorS("Failed to compute initial matrix");
        return NULL;
    }
    //std::cout << "[DEBUG] Computed initial matrix " << MATROWS(result) << "x" << MATCOLS(result) << std::endl;

    // Create new ring RZ with characteristic p
    //std::cout << "[DEBUG] Creating new ring with characteristic " << p << std::endl;
    
   ring RZ = createFiniteFieldRing(R, p);

    ring savedRing = currRing;
    rChangeCurrRing(RZ);

    // Setup mapping parameters
    int nvars = rVar(R);
    int npars = rPar(R);
    int* perm = (int*)omAlloc0((nvars + 1) * sizeof(int));
    int* par_perm = (int*)omAlloc0((npars + 1) * sizeof(int));
    
   
    
    // Setup identity permutation for variables
    for (int i = 1; i <= nvars; i++) {
        perm[i] = i;
    }
    
    // Setup identity permutation for parameters
    for (int i = 0; i < npars; i++) {
        par_perm[i] = i + 1;
    }
    std::cout << "[DEBUG] Set up permutations" << std::endl;

    // Map matrix using id_PermIdeal
    nMapFunc nMap = n_SetMap(R->cf, RZ->cf);
    if (!nMap) {
        rChangeCurrRing(savedRing);
        omFree(perm);
        omFree(par_perm);
        WerrorS("Failed to create number map");
        return NULL;
    }
    
    // Get matrix dimensions
    int R_rows = result->nrows;
    int R_cols = result->ncols;
    std::cout << "[DEBUG] Matrix dimensions: " << R_rows << "x" << R_cols << std::endl;

    // Map matrix using id_PermIdeal with correct parameters
    ideal mapped_ideal = id_PermIdeal(
        (ideal)result,  // input ideal/matrix
        R_rows,        // number of rows
        R_cols,        // number of columns
        perm,          // variable permutation
        R,            // source ring
        RZ,           // destination ring
        nMap,         // number mapping function
        par_perm,     // parameter permutation
        npars,        // number of parameters
        FALSE         // use multiplication flag
    );

    if (!mapped_ideal) {
        rChangeCurrRing(savedRing);
        omFree(perm);
        omFree(par_perm);
        WerrorS("Failed to map ideal");
        return NULL;
    }

    // Convert back to matrix with correct dimensions
    matrix mapped_result = (matrix)mapped_ideal;
    mapped_result->nrows = R_rows;
    mapped_result->ncols = R_cols;
    std::cout << "[DEBUG] Matrix mapped successfully" << std::endl;

    // Create output list
    lists output = (lists)omAlloc0Bin(slists_bin);
    if (!output) {
        rChangeCurrRing(savedRing);
        id_Delete(&mapped_ideal, RZ);
        omFree(perm);
        omFree(par_perm);
        WerrorS("Failed to allocate output list");
        return NULL;
    }
    
    output->Init(4);

    lists t1 = (lists)omAlloc0Bin(slists_bin);
    if (!t1) {
        rChangeCurrRing(savedRing);
        id_Delete(&mapped_ideal, RZ);
        omFree(perm);
        omFree(par_perm);
        omFreeBin(output, slists_bin);
        WerrorS("Failed to allocate t1 list");
        return NULL;
    }
    
    t1->Init(2);

    // Initialize strings first to ensure they're properly allocated
    char* gen_str = omStrDup("generators");
    char* setmat_str = omStrDup("setMat");
    if (!gen_str || !setmat_str) {
        if (gen_str) omFree(gen_str);
        if (setmat_str) omFree(setmat_str);
        rChangeCurrRing(savedRing);
        id_Delete(&mapped_ideal, RZ);
        omFree(perm);
        omFree(par_perm);
        omFreeBin(t1, slists_bin);
        omFreeBin(output, slists_bin);
        WerrorS("Failed to allocate strings");
        return NULL;
    }

    t1->m[0].rtyp = STRING_CMD;
    t1->m[0].data = gen_str;
    t1->m[1].rtyp = STRING_CMD;
    t1->m[1].data = setmat_str;

    // Make sure RZ is complete and valid
    rComplete(RZ);
    
    // Set up output structure
    output->m[0].rtyp = RING_CMD;
    output->m[0].data = currRing;  // Create a copy of the ring
    output->m[1].rtyp = LIST_CMD;
    output->m[1].data = t1;
    output->m[2].rtyp = RING_CMD;
    output->m[2].data = currRing;  // Create another copy of the ring
    output->m[3].rtyp = MATRIX_CMD;
    output->m[3].data = mp_Copy(mapped_result, RZ);  // Create a copy of the matrix
    

    // Clean up original objects
    id_Delete(&mapped_ideal, RZ);
    omFree(perm);
    omFree(par_perm);

    // Clean up IBP structure
    for (int i = 0; i <= ibp->nr; i++) {
        if (S->IBP[i]) {
            omFreeSize(S->IBP[i], sizeof(*S->IBP[i]));
        }
    }
    omFreeSize(S->IBP, sizeof(oneIBP) * (ibp->nr + 2));
    omFreeSize(S, sizeof(*S));

    // Restore original ring
    rChangeCurrRing(savedRing);

    std::cout << "[DEBUG] Output structure created and validated" << std::endl;
     // Extract input data
   
    lists matrix_output = (lists)output->m[3].data;
    std::cout << "printing matrix_output" << listToString(matrix_output) << std::endl;
    return output;
}

std::string singular_setMat(std::string const& res,
    std::string const& res1,
    int const& j,
    std::string const& needed_library,
    std::string const& base_filename)
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
    void* p = (char*)(long)(j);

    Res = deserialize(res, ids);
    Res1 = deserialize(res1, ids);

    ScopedLeftv args(Res.first, lCopy(Res.second));
    ScopedLeftv args1(args, Res1.first, lCopy(Res1.second));
    ScopedLeftv arg(args1, INT_CMD, p);
    // Call the setMat function
    lists result = setMat(args.leftV());
    
    // Serialize the result
    std::string output_filename = serialize(result, base_filename);
    
    
    return output_filename;
}

// Function to create finite field ring
ring createFiniteFieldRing(ring R, int p) {
    // Calculate number of t parameters (m2-1)
    int m = rPar(R);  // number of parameters in original ring
    int m2 = (m * (m - 1)) / 2;  // total number of t parameters needed
    int mt = m2 > 0 ? m2 - 1 : 0;  // number of t parameters to create
    
    // Create parameter names array (t(1)...t(mt),D)
    char **par_names = (char**)omAlloc0((mt + 2) * sizeof(char*));  // +2 for D and NULL terminator
    if (!par_names) {
        WerrorS("Failed to allocate parameter names array");
        return NULL;
    }
    
    // Create t parameter names
    for (int i = 0; i < mt; i++) {
        std::string name = "t(" + std::to_string(i + 1) + ")";
        par_names[i] = omStrDup(name.c_str());
        if (!par_names[i]) {
            for (int j = 0; j < i; j++) omFree(par_names[j]);
            omFree(par_names);
            WerrorS("Failed to create parameter name");
            return NULL;
        }
    }
    
    // Add D parameter
    par_names[mt] = omStrDup("D");
    if (!par_names[mt]) {
        for (int i = 0; i < mt; i++) omFree(par_names[i]);
        omFree(par_names);
        WerrorS("Failed to create D parameter");
        return NULL;
    }
    
    // Create variable names array (z variables)
    int n = rVar(R);  // number of variables
    char **vars = (char**)omAlloc0((n + 1) * sizeof(char*));
    if (!vars) {
        for (int i = 0; i <= mt; i++) omFree(par_names[i]);
        omFree(par_names);
        WerrorS("Failed to allocate variable names array");
        return NULL;
    }
    
    // Create z variable names
    for (int i = 0; i < n; i++) {
        std::string name = "z(" + std::to_string(i + 1) + ")";
        vars[i] = omStrDup(name.c_str());
        if (!vars[i]) {
            for (int j = 0; j < i; j++) omFree(vars[j]);
            omFree(vars);
            for (int j = 0; j <= mt; j++) omFree(par_names[j]);
            omFree(par_names);
            WerrorS("Failed to create variable name");
            return NULL;
        }
    }

    // Create base coefficient field with characteristic p
    coeffs cf = nInitChar(n_Zp, (void*)(long)p);
    if (!cf) {
        for (int i = 0; i < n; i++) omFree(vars[i]);
        omFree(vars);
        for (int i = 0; i <= mt; i++) omFree(par_names[i]);
        omFree(par_names);
        WerrorS("Could not create coefficient field");
        return NULL;
    }

    // Create parameter ring
    ring paramRing = rDefault(cf, mt + 1, par_names, ringorder_dp);
    if (!paramRing) {
        nKillChar(cf);
        for (int i = 0; i < n; i++) omFree(vars[i]);
        omFree(vars);
        for (int i = 0; i <= mt; i++) omFree(par_names[i]);
        omFree(par_names);
        WerrorS("Could not create parameter ring");
        return NULL;
    }
    rComplete(paramRing);

    // Create extension field
    TransExtInfo extParam;
    extParam.r = paramRing;
    coeffs extCf = nInitChar(n_transExt, &extParam);
    if (!extCf) {
        rKill(paramRing);
        for (int i = 0; i < n; i++) omFree(vars[i]);
        omFree(vars);
        for (int i = 0; i <= mt; i++) omFree(par_names[i]);
        omFree(par_names);
        WerrorS("Could not create extension field");
        return NULL;
    }

    // Create final ring RZ with dp ordering
    ring RZ = rDefault(extCf, n, vars, ringorder_dp);
    if (!RZ) {
        nKillChar(extCf);
        rKill(paramRing);
        for (int i = 0; i < n; i++) omFree(vars[i]);
        omFree(vars);
        for (int i = 0; i <= mt; i++) omFree(par_names[i]);
        omFree(par_names);
        WerrorS("Could not create final ring");
        return NULL;
    }
    rComplete(RZ);

    // Clean up name arrays
    for (int i = 0; i < n; i++) omFree(vars[i]);
    omFree(vars);
    for (int i = 0; i <= mt; i++) omFree(par_names[i]);
    omFree(par_names);

    return RZ;
}
/* 
lists getRedIBPs_with_mapping(setIBP S, int p, ring R) {
    // Generate random values for parameters
    lists input_val = getRandom(p, rPar(R));
    std::cout << "input_val: " << listToString(input_val) << std::endl;
    
    // Get sorted integrals and random values
    lists ind = flint_getSortedIntegrals(S);
    std::cout << "ind: " << listToString(ind) << std::endl;
    
    // Create matrix using setMat
    matrix N = setMat(S, input_val, ind, R);
    
    // Create finite field ring and map matrix
    ring RZ = createFiniteFieldRing(R, p);
    std::cout << "RZ: " << rString(RZ) << std::endl;
    
    // Save current ring
    ring savedRing = currRing;
    std::cout << "savedRing: " << rString(savedRing) << std::endl;
    rChangeCurrRing(RZ);
    
    // Map matrix to finite field
    // Setup mapping parameters
    int nvars1 = rVar(R);
    int npars1 = rPar(R);
    int* perm1 = (int*)omAlloc0((nvars1 + 1) * sizeof(int));
    int* par_perm1 = (int*)omAlloc0((npars1 + 1) * sizeof(int));
    
    // Setup identity permutation for variables
    for (int i = 1; i <= nvars1; i++) {
        perm1[i] = i;
    }
    
    // Setup identity permutation for parameters
    for (int i = 0; i < npars1; i++) {
        par_perm1[i] = i + 1;
    }
    
    // Map matrix using id_PermIdeal
    nMapFunc nMap1 = n_SetMap(R->cf, RZ->cf);
    
    // Get matrix dimensions
    int R_rows1 = N->nrows;
    int R_cols1 = N->ncols;
    
    // Map matrix using id_PermIdeal with correct parameters
    ideal mapped_ideal1 = id_PermIdeal(
        (ideal)N,  // input ideal/matrix
        R_rows1,    // number of rows
        R_cols1,    // number of columns
        perm1,      // variable permutation
        R,         // source ring
        RZ,        // destination ring
        nMap1,      // number mapping function
        par_perm1,  // parameter permutation
        npars1,     // number of parameters
        FALSE      // use multiplication flag
    );
    
    // Convert back to matrix with correct dimensions
    matrix mapped_result1 = (matrix)mapped_ideal1;
    mapped_result1->nrows = R_rows1;
    mapped_result1->ncols = R_cols1;
    
    // Perform Gauss reduction
    std::cout << "mapped_result1: " << std::endl;
    lists Z1 = singflint_rref_full(mapped_result1, RZ, p);

    matrix P = (matrix)Z1->m[0].data;
    matrix U = (matrix)Z1->m[1].data;
    matrix M = (matrix)Z1->m[2].data;
    int rank = (int)(long)Z1->m[3].data; 
     std::cout<<" before mapping P"<<std::endl;
    printMat(P);
    std::cout<<" before mapping U"<<std::endl;
    printMat(U);
    std::cout<<" before mapping M"<<std::endl;
    printMat(M);
 
    // Debug output
    std::cout << "P size: " << MATROWS(P) << "x" << MATCOLS(P) << std::endl;
    std::cout << "U size: " << MATROWS(U) << "x" << MATCOLS(U) << std::endl;
    std::cout << "M size: " << MATROWS(M) << "x" << MATCOLS(M) << std::endl;
    std::cout << "Rank: " << rank << std::endl;
    std::cout << "Current ring: " << rString(currRing) << ", RZ: " << rString(RZ) << std::endl;
 
    // Map back to original ring
    rChangeCurrRing(R);
    
    // Create new list structure for mapped result
    lists ZR = (lists)omAllocBin(slists_bin);
    ZR->Init(4); // P, U, S, rank
    
    // Map each matrix component
    for(int i = 0; i < 3; i++) { // Map P, U, S matrices
        matrix mat = (matrix)Z1->m[i].data;
        if (!mat) {
            WerrorS("Invalid matrix in Z1");
            return NULL;
        }
        
        // Setup mapping parameters for each matrix
        int nvars_mat = rVar(R);
        int npars_mat = rPar(R);
        int* perm_mat = (int*)omAlloc0((nvars_mat + 1) * sizeof(int));
        int* par_perm_mat = (int*)omAlloc0((npars_mat + 1) * sizeof(int));
        
        if (!perm_mat || !par_perm_mat) {
            rChangeCurrRing(savedRing);
            if (perm_mat) omFree(perm_mat);
            if (par_perm_mat) omFree(par_perm_mat);
            WerrorS("Failed to allocate permutation arrays");
            return NULL;
        }
        
        // Setup identity permutation for variables
        for (int j = 1; j <= nvars_mat; j++) {
            perm_mat[j] = j;
        }
        
        // Setup identity permutation for parameters
        for (int j = 0; j < npars_mat; j++) {
            par_perm_mat[j] = j + 1;
        }
        
        // Map matrix using id_PermIdeal
        nMapFunc nMap_mat = n_SetMap(RZ->cf, R->cf);
        if (!nMap_mat) {
            rChangeCurrRing(savedRing);
            omFree(perm_mat);
            omFree(par_perm_mat);
            WerrorS("Failed to create number map");
            return NULL;
        }
        
        // Get matrix dimensions
        int R_rows_mat = mat->nrows;
        int R_cols_mat = mat->ncols;
        
        // Map matrix using id_PermIdeal with correct parameters
        ideal mapped_ideal_mat = id_PermIdeal(
            (ideal)mat,  // input ideal/matrix
            R_rows_mat,  // number of rows
            R_cols_mat,  // number of columns
            perm_mat,    // variable permutation
            RZ,         // source ring
            R,          // destination ring
            nMap_mat,   // number mapping function
            par_perm_mat, // parameter permutation
            npars_mat,  // number of parameters
            FALSE       // use multiplication flag
        );
        
        if (!mapped_ideal_mat) {
            rChangeCurrRing(savedRing);
            omFree(perm_mat);
            omFree(par_perm_mat);
            WerrorS("Failed to map ideal");
            return NULL;
        }
        
        // Convert back to matrix with correct dimensions
        matrix mapped_result_mat = (matrix)mapped_ideal_mat;
        mapped_result_mat->nrows = R_rows_mat;
        mapped_result_mat->ncols = R_cols_mat;
        
        // Store in ZR
        ZR->m[i].rtyp = MATRIX_CMD;
        ZR->m[i].data = mapped_result_mat;
        
        // Clean up
        omFree(perm_mat);
        omFree(par_perm_mat);
    }

   

    // Clean up
    rChangeCurrRing(savedRing);
    rKill(RZ);
    
    // Extract reduced IBPs
    setIBP indIBP = (setIBP)omAlloc0(sizeof(*indIBP));
    indIBP->over = R;  // Set the ring
    
    // Count valid IBPs in S
    int valid_ibp_count = 0;
    while (S->IBP[valid_ibp_count] != NULL) {
        valid_ibp_count++;
    }
    
    // Get matrices from ZR
    matrix P_mat = (matrix)ZR->m[0].data;
    matrix S_mat = (matrix)ZR->m[2].data;
    
    // Allocate space for IBPs with maximum possible size
    indIBP->IBP = (oneIBP*)omAlloc0(sizeof(oneIBP) * (valid_ibp_count + 1));
    
    // Process IBPs directly like in setmat.lib
    int l = 0;
    for(int j = 1; j <= rank; j++) {
        for(int k = 1; k <= MATCOLS(P_mat) && k <= valid_ibp_count; k++) {
            poly elem = MATELEM(P_mat, j, k);
            if(!elem || n_IsZero(pGetCoeff(elem), currRing->cf)) {
                continue;
            }
            
            // Check source IBP validity
            if(!S->IBP[k-1] || !S->IBP[k-1]->i || !S->IBP[k-1]->c) {
                continue;
            }
            
            // Print source IBP info
            std::cout << "S.IBP[" << k-1 << "]: i=<list>" << std::endl;
            std::cout << "c=<list>" << std::endl;
            
            // Allocate and copy IBP
            indIBP->IBP[l] = (oneIBP)omAlloc0(sizeof(*S->IBP[0]));
            if(!indIBP->IBP[l]) {
                continue;
            }
            
            // Deep copy the lists
            indIBP->IBP[l]->i = lCopy(S->IBP[k-1]->i);
            indIBP->IBP[l]->c = lCopy(S->IBP[k-1]->c);
            
            if(!indIBP->IBP[l]->i || !indIBP->IBP[l]->c) {
                // Clean up on failure
                if(indIBP->IBP[l]->i) omFreeBin(indIBP->IBP[l]->i, slists_bin);
                if(indIBP->IBP[l]->c) omFreeBin(indIBP->IBP[l]->c, slists_bin);
                omFreeSize(indIBP->IBP[l], sizeof(*S->IBP[0]));
                indIBP->IBP[l] = NULL;
                continue;
            }
            
            // Print copied IBP info
            std::cout << "indIBP[" << l << "].c: " << listToString(indIBP->IBP[l]->c) << std::endl;
            std::cout << "indIBP[" << l << "].i: " << listToString(indIBP->IBP[l]->i) << std::endl;
            
            l++;
        }
    }
    
    // Null terminate the IBP array
    indIBP->IBP[l] = NULL;
    
    // Create list for seeds and initialize with correct size
    lists seeds = (lists)omAllocBin(slists_bin);
    seeds->Init(rank);
    
    // Process seeds directly from S_mat
    l = 0;  // Reset counter for seeds
    for(int k = 1; k <= MATCOLS(S_mat) && l < rank; k++) {
        bool found_nonzero = false;
        for(int j = 1; j <= MATROWS(S_mat); j++) {
            poly elem = MATELEM(S_mat, j, k);
            if(elem && !n_IsZero(pGetCoeff(elem), currRing->cf)) {
                found_nonzero = true;
                break;
            }
        }
        
        if(found_nonzero && k-1 < ind->nr + 1) {
            lists ind_k = (lists)ind->m[k-1].data;
            if(ind_k) {
                seeds->m[l].rtyp = LIST_CMD;
                seeds->m[l].data = lCopy(ind_k);
                l++;
            }
        }
    }
    
    // Fill remaining seeds with NULL if necessary
    for(int i = l; i < rank; i++) {
        seeds->m[i].rtyp = LIST_CMD;
        seeds->m[i].data = NULL;
    }
    
    // Create result list
    lists result = (lists)omAllocBin(slists_bin);
    result->Init(2);
    result->m[0].rtyp = LIST_CMD;
    result->m[0].data = (void*)indIBP;
    result->m[1].rtyp = LIST_CMD;
    result->m[1].data = (void*)seeds;
    
    return result;
} 
*/









lists getRedIBPs(setIBP S, int p, ring R) {
    // Generate random values for parameters
    lists input_val = getRandom(p, rPar(R));
    //std::cout << "input_val: " << listToString(input_val) << std::endl;
    
    // Get sorted integrals and random values
    lists ind = flint_getSortedIntegrals(S);
    //std::cout<<"ind->nr+1: "<<ind->nr+1<<std::endl;
    // Create matrix using setMat
    matrix N = setMat(S, input_val, ind, R);
   
   
    
    // Create result list
    lists result = (lists)omAllocBin(slists_bin);
    result->Init(3);
    result->m[0].rtyp = LIST_CMD;
    result->m[0].data = ind;
    result->m[1].rtyp = LIST_CMD;
    result->m[1].data = input_val;
    result->m[2].rtyp = MATRIX_CMD;
    result->m[2].data = N;
    
    return result;
}
lists getRedIBPs_wrapper(leftv args) {
    lists input_S = (lists)args->Data();
    lists Set = (lists)input_S->m[3].data;

    ring R = (ring)Set->m[0].data;
    //  std::cout << "R: " << rString(R) << std::endl;
   
    int p = (int)(long)args->next->Data();
    //std::cout << "p: " << p << std::endl;
    // Extract IBP and index lists
    lists ibp = (lists)Set->m[3].data;
    // Create setIBP structure
    setIBP S = (setIBP)omAlloc0(sizeof(*S));
    
    S->IBP = (oneIBP*)omAlloc0(sizeof(oneIBP) * (ibp->nr + 2));
    if (!S->IBP) {
        omFreeSize(S, sizeof(*S));
        WerrorS("Memory allocation failed for IBP array");
        return NULL;
    }
    
    // Copy IBP data
    int validIBPs = 0;
    for (int i = 0; i <= ibp->nr; i++) {
        if (ibp->m[i].rtyp == LIST_CMD && ibp->m[i].data) {
            lists ibp_item = (lists)ibp->m[i].data;
            S->IBP[validIBPs] = (oneIBP)omAlloc0(sizeof(*S->IBP[validIBPs]));
            
            if (ibp_item->nr >= 1) {
                S->IBP[validIBPs]->c = (lists)ibp_item->m[0].data;
                S->IBP[validIBPs]->i = (lists)ibp_item->m[1].data;
                validIBPs++;
            }
        }
    }
    S->IBP[validIBPs] = NULL;
    S->over = R;
   
    lists result = getRedIBPs(S, p, R);
//std::cout<<"printing result"<<std::endl;
lists ind_result = (lists)result->m[0].data;
lists val_result = (lists)result->m[1].data;
matrix N_result = (matrix)result->m[2].data;/* 
std::cout<<"ind_result: "<<listToString(ind_result)<<std::endl;
std::cout<<"val_result: "<<listToString(val_result)<<std::endl;
std::cout<<"N_result: "<<std::endl;
printMat(N_result);

 */

    // Prepare the output token
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(4);
     lists t1 = (lists)omAlloc0Bin(slists_bin);
    t1->Init(2);
t1->m[0].rtyp = STRING_CMD; t1->m[0].data = strdup("generators");
t1->m[1].rtyp = STRING_CMD; t1->m[1].data = strdup("getRedIBPs");
    
output->m[0].rtyp = RING_CMD; output->m[0].data = currRing;
    output->m[1].rtyp = LIST_CMD; output->m[1].data = t1;
output->m[2].rtyp = RING_CMD; output->m[2].data = currRing;
    
// Create and initialize the second sublist for Li elements
    lists t2 = (lists)omAlloc0Bin(slists_bin);
t2->Init(3); // Use the size of Li to initialize t2
    t2->m[0].rtyp = LIST_CMD; 
    t2->m[0].data = lCopy((lists)ind_result);
    t2->m[1].rtyp = LIST_CMD; 
    t2->m[1].data = lCopy((lists)val_result);
    t2->m[2].rtyp = MATRIX_CMD; 
    t2->m[2].data = N_result;


    output->m[3].rtyp = LIST_CMD; output->m[3].data = t2;
    return output;
}


std::string singular_getRedIBPs_wrapper(std::string const& res
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

    lists result = getRedIBPs_wrapper(args.leftV());

    out_filename = serialize(result, base_filename);

    return out_filename;
}

std::string singular_prepareRedIBPs_gpi_cpp(std::string const& res
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
    std::string function_name = "prepareRedIBPs_gpi_cpp";
    out = call_user_proc(function_name, needed_library, args);
    out_filename = serialize(out.second, base_filename);

    return out_filename;
}



std::string singular_computeSector_flint_cpp(
    std::string const& labeledgraph_str,
    std::string const& userInput_str,
    std::string const& labels_11_str,
    int const& seed,
    std::string const& needed_library,
    std::string const& base_filename)
{
    init_singular(config::singularLibrary().string());
    load_singular_library(needed_library);

    std::pair<int, lists> labeledgraph;
    std::pair<int, lists> userInput;
    std::pair<int, lists> labels_11;
    std::pair<int, lists> out;
    std::string ids;
    ids = worker();
    labeledgraph = deserialize(labeledgraph_str, ids); // labeledgraph
    userInput = deserialize(userInput_str, ids);       // userInput
    labels_11 = deserialize(labels_11_str, ids);      // labels_11
    void* p = (char*)(long)(seed);                    // 7853

    ScopedLeftv labeledgraph_args(labeledgraph.first, lCopy(labeledgraph.second)); // labeledgraph
    ScopedLeftv userInput_args(labeledgraph_args, userInput.first, lCopy(userInput.second)); // userInput
    ScopedLeftv labels_11_args(userInput_args, labels_11.first, lCopy(labels_11.second)); // labels_11

    // Timing for target ints computation
    auto target_start_time = std::chrono::high_resolution_clock::now(); 
    std::string function_name = "returnTargetInts_gpi_cpp";
   // std::cout<<"*********computing target ints**********"<<std::endl;
    out = call_user_proc(function_name, needed_library, labeledgraph_args);
    std::string one_sector_11_filename = serialize(out.second, base_filename); // one_sector_11
    auto target_end_time = std::chrono::high_resolution_clock::now();
    auto target_duration = std::chrono::duration_cast<std::chrono::milliseconds>(target_end_time - target_start_time);
    std::cout << "Computing target ints completed  in " << target_duration.count() << " milliseconds\n";
    std::pair<int, lists> one_sector_11 = deserialize(one_sector_11_filename, ids); // one_sector_11

    // First get reduced IBP system
    ScopedLeftv one_sector_11_args(one_sector_11.first, lCopy(one_sector_11.second)); // one_sector_11
    ScopedLeftv seed_args(one_sector_11_args, INT_CMD, p); // seed (7853)
    //std::cout<<"*********getting red ibps**********"<<std::endl;
    
    // Timing for getting red IBPs
    auto getredibps_start_time = std::chrono::high_resolution_clock::now();    
   lists out1 =  getRedIBPs_wrapper(one_sector_11_args.leftV());
    std::string getredibps_filename = serialize(out1, base_filename); // getredibps
    auto getredibps_end_time = std::chrono::high_resolution_clock::now();
    auto getredibps_duration = std::chrono::duration_cast<std::chrono::milliseconds>(getredibps_end_time - getredibps_start_time);
    std::cout << "Getting red ibps completed in " << getredibps_duration.count() << " milliseconds\n";
    std::filesystem::remove(one_sector_11_filename); // Clean up one_sector_11_filename
    //std::cout<<"current ring after getting red ibps: "<<rString(currRing)<<std::endl;
  ring savedRing = currRing;

    std::pair<int, lists> getredibps = deserialize(getredibps_filename, ids); // getredibps
    ScopedLeftv getredibps_args(getredibps.first, lCopy(getredibps.second)); // getredibps
    ScopedLeftv seed_args2(getredibps_args, INT_CMD, p); // seed (7853)
    
    // Now prepare reduced IBP system
    std::string function_name2 = "prepareRedIBPs_gpi_cpp";
    std::cout<<"*********preparing red ibps**********"<<std::endl;
    
    // Timing for preparing red IBPs
    auto prep_start_time = std::chrono::high_resolution_clock::now();    
    out = call_user_proc(function_name2, needed_library, getredibps_args);
    std::string prep_filename = serialize(out.second, base_filename); // prep
    auto prep_end_time = std::chrono::high_resolution_clock::now();
    auto prep_duration = std::chrono::duration_cast<std::chrono::milliseconds>(prep_end_time - prep_start_time);
    std::cout << "Preparing red ibps completed in " << prep_duration.count() << " milliseconds\n";
    std::filesystem::remove(getredibps_filename); // Clean up getredibps_filename

    std::pair<int, lists> prep = deserialize(prep_filename, ids); // prep
    ScopedLeftv prep_args(prep.first, lCopy(prep.second)); // prep
    //std::cout<<"current ring before gauss reduction: "<<rString(currRing)<<std::endl;

    // Timing for Gauss reduction
    //std::cout<<"********performing gauss reduction*********"<<std::endl;    
    auto gauss_start_time = std::chrono::high_resolution_clock::now();
    lists result = singflintGaussRed(prep_args.leftV());
    auto gauss_end_time = std::chrono::high_resolution_clock::now();
    auto gauss_duration = std::chrono::duration_cast<std::chrono::milliseconds>(gauss_end_time - gauss_start_time);
    std::cout << "Gauss reduction completed in " << gauss_duration.count() << " milliseconds\n";
  
    std::string gaussred_filename = serialize(result, base_filename); // gaussred
    std::filesystem::remove(prep_filename); // Clean up prep_filename

    std::pair<int, lists> gaussred = deserialize(gaussred_filename, ids); // gaussred

    ScopedLeftv one_sector_11_args2(one_sector_11.first, lCopy(one_sector_11.second)); // one_sector_11
    ScopedLeftv gaussred_args(one_sector_11_args2, gaussred.first, lCopy(gaussred.second)); // gaussred
    ScopedLeftv prep_args2(gaussred_args, prep.first, lCopy(prep.second)); // prep
    ScopedLeftv seed_args3(prep_args2, INT_CMD, p); // seed (7853)
    rChangeCurrRing(savedRing);
    //std::cout<<"current ring after changing ring: "<<rString(currRing)<<std::endl;
    // Timing for computing red IBPs
    //std::cout<<"computing get red ibps*******"<<std::endl;
    auto redibps_start_time = std::chrono::high_resolution_clock::now();
    //std::cout<<"current ring: "<<rString(currRing)<<std::endl;
    std::string function_name3 = "computeGetRedIBPs_gpi_cpp";
    
    // Call the function with the first argument in the chain
    out = call_user_proc(function_name3, needed_library, one_sector_11_args2);
    
    //std::cout<<"after compute get red ibps"<<std::endl;
    std::string getredibps_final_filename = serialize(out.second, base_filename); // getredibps
    auto redibps_end_time = std::chrono::high_resolution_clock::now();
    auto redibps_duration = std::chrono::duration_cast<std::chrono::milliseconds>(redibps_end_time - redibps_start_time);
    std::cout << "Computing get red ibps completed in " << redibps_duration.count() << " milliseconds\n";
    std::filesystem::remove(gaussred_filename); // Clean up gaussred_filename

    std::pair<int, lists> getredibps_final = deserialize(getredibps_final_filename, ids); // getredibps

    ScopedLeftv labeledgraph_args2(labeledgraph.first, lCopy(labeledgraph.second)); // labeledgraph
    ScopedLeftv getredibps_final_args(labeledgraph_args2, getredibps_final.first, lCopy(getredibps_final.second)); // getredibps
    ScopedLeftv labels_11_args2(getredibps_final_args, labels_11.first, lCopy(labels_11.second)); // labels_11
    
    // Timing for final computation
    auto final_start_time = std::chrono::high_resolution_clock::now();
    std::string function_name4 = "ComputeOneSector_gpi";
    out = call_user_proc(function_name4, needed_library, labeledgraph_args2);
    std::string ComputeOneSector_filename = serialize(out.second, base_filename); // ComputeOneSector
    auto final_end_time = std::chrono::high_resolution_clock::now();
    auto final_duration = std::chrono::duration_cast<std::chrono::milliseconds>(final_end_time - final_start_time);
    std::cout << "Computing one sector completed in " << final_duration.count() << " milliseconds\n";
    std::filesystem::remove(getredibps_final_filename); // Clean up getredibps_final_filename

    return ComputeOneSector_filename;
}

