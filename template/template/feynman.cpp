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
    for (int i = 0; i <= L->nr; i++) {
        if (L->m[i].rtyp == INT_CMD) {
            s += std::to_string((int)(long)L->m[i].data) + ",";
        }
    }
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
            s += abs(val);
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
    result->m[3].data = (void*)(long)s;

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
    if (!L1 || !L2) return L1 == L2 ? 0 : (L1 ? 1 : -1);
    int min_size = std::min(L1->nr, L2->nr);
    for (int i = 0; i <= min_size; i++) {
        if (L1->m[i].rtyp != L2->m[i].rtyp) return L1->m[i].rtyp < L2->m[i].rtyp ? -1 : 1;
        if (L1->m[i].rtyp == INT_CMD) {
            int v1 = (int)(long)L1->m[i].data;
            int v2 = (int)(long)L2->m[i].data;
            if (v1 != v2) return v1 < v2 ? -1 : 1;
        }
    }
    return L1->nr < L2->nr ? -1 : (L1->nr > L2->nr ? 1 : 0);
}

lists flint_getSortedIntegrals(setIBP S) {
    std::cout << "DEBUG: Entering flint_getSortedIntegrals" << std::endl;
    
    if (!S) {
        WerrorS("NULL S in flint_getSortedIntegrals");
        return NULL;
    }
    if (!S->IBP) {
        WerrorS("NULL S->IBP in flint_getSortedIntegrals");
        return NULL;
    }
    std::cout << "DEBUG: Input validation passed" << std::endl;

    // Count total number of indices and debug IBP structure
    int total_indices = 0;
    int ibp_count = 0;
    while (S->IBP[ibp_count] != NULL) {
        if (S->IBP[ibp_count]->i) {
            total_indices += S->IBP[ibp_count]->i->nr + 1;
            std::cout << "DEBUG: IBP[" << (ibp_count + 1) << "] has " << (S->IBP[ibp_count]->i->nr + 1) << " seeds: " << listToString(S->IBP[ibp_count]->i) << std::endl;
        }
        ibp_count++;
    }
    std::cout << "DEBUG: Found " << ibp_count << " IBP relations with " << total_indices << " total indices" << std::endl;

    // Initialize pairs list
    int capacity = std::max(16, total_indices / 4);
    lists pairs = (lists)omAlloc0Bin(slists_bin);
    if (!pairs) {
        WerrorS("Memory allocation failed for pairs");
        return NULL;
    }
    pairs->Init(capacity);
    pairs->nr = -1;
    std::cout << "DEBUG: pairs initialized with capacity " << capacity << std::endl;

    // Use hash set for duplicate checking
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

            // Debug print for indv
            std::cout << "indv=oneI.i[" << (k + 1) << "]: " << listToString(ind) << std::endl;

            // Check for duplicates
            if (seen_indices.find(serial) == seen_indices.end()) {
                seen_indices.insert(serial);

                // Compute sort measures
                lists measures = getSortMeasures(ind);
                if (!measures) {
                    omFreeBin(pairs, slists_bin);
                    WerrorS("Failed to compute sort measures");
                    return NULL;
                }

                // Debug print for getSortMeasures
                std::cout << "getSortMeasures(indv): " << listToString(measures) << std::endl;

                // Resize pairs if necessary
                if (pairs->nr + 1 >= capacity) {
                    int new_capacity = capacity * 2;
                    lists new_pairs = (lists)omAlloc0Bin(slists_bin);
                    if (!new_pairs) {
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
                    std::cout << "DEBUG: Resized pairs to capacity " << capacity << std::endl;
                }

                // Add pair (ind, measures)
                pairs->nr++;
                lists pair = (lists)omAlloc0Bin(slists_bin);
                if (!pair) {
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

            std::sort(sorted_pairs.begin(), sorted_pairs.end(), [](lists a, lists b) {
                if (!a || !b || !a->m[1].data || !b->m[1].data) return false;
                return compareListsLex((lists)a->m[1].data, (lists)b->m[1].data) < 0;
            });

            // Add sorted pairs to result
            for (int i = 0; i <= pairs->nr; i++) {
                if (!sorted_pairs[i]) continue;
                lists pair = sorted_pairs[i];
                result->nr++;
                result->m[result->nr].rtyp = LIST_CMD;
                result->m[result->nr].data = lCopy(pair);
            }

            // Clean up pairs
            for (int i = 0; i <= pairs->nr; i++) {
                lists pair = (lists)pairs->m[i].data;
                if (pair) {
                    if (pair->m[0].data) {
                        lists L = (lists)pair->m[0].data;
                        for (int j = 0; j <= L->nr; j++) {
                            L->m[j].CleanUp();
                        }
                        omFreeBin(L, slists_bin);
                        pair->m[0].data = NULL;
                    }
                    if (pair->m[1].data) {
                        lists L = (lists)pair->m[1].data;
                        for (int j = 0; j <= L->nr; j++) {
                            L->m[j].CleanUp();
                        }
                        omFreeBin(L, slists_bin);
                        pair->m[1].data = NULL;
                    }
                    omFreeBin(pair, slists_bin);
                }
            }
        } catch (const std::exception& e) {
            std::cout << "DEBUG: Exception during sorting: " << e.what() << std::endl;
            omFreeBin(result, slists_bin);
            result = (lists)omAlloc0Bin(slists_bin);
            result->Init(0);
        }
    }

    omFreeBin(pairs, slists_bin);
    std::cout << "DEBUG: Returning result with " << (result->nr + 1) << " elements" << std::endl;
    // Print result in Singular-compatible flat format
    std::string result_str;
    for (int i = 0; i <= result->nr; i++) {
        if (result->m[i].rtyp == LIST_CMD && result->m[i].data) {
            lists pair = (lists)result->m[i].data;
            result_str += listToString((lists)pair->m[0].data) + "," + listToString((lists)pair->m[1].data);
            if (i < result->nr) result_str += ",";
        }
    }
    std::cout << "DEBUG: Result: " << result_str << std::endl;
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
//std::cout<<"first element of S.IBP: "<<listToString((lists)S->IBP[0]->i)<<std::endl;

//std::cout<<"size of S.IBP: "<<ibp->nr+1<<std::endl;
    // Call main function
    lists result = flint_getSortedIntegrals(S);
    lists result1=(lists)result->m[0].data;
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
    std::cout<<"result3="<<listToString(result3)<<std::endl;
    //std::cout << "DEBUG: flint_getSortedIntegrals completed" << std::endl;

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
    
   
    out_filename = serialize(result, base_filename);
    
    return out_filename;
}


// Helper function to compare two lists lexicographically (comp_lex)
static int comp_lex(lists l1, lists l2) {
    if (!l1 || !l2) return 0;
    
    // Get the actual size of the lists
    int size1 = l1->nr + 1;
    int size2 = l2->nr + 1;
    int n = (size1 < size2) ? size1 : size2;
    
    std::cout<<"l1="<<listToString(l1)<<std::endl;
    std::cout<<"l2="<<listToString(l2)<<std::endl;
    std::cout << "comp_lex: comparing lists of size " << size1 << " and " << size2 << std::endl;
    
    // Compare each element
    for (int i = 0; i < n; i++) {
        // Skip if either element is not an integer
        if (l1->m[i].rtyp != INT_CMD || l2->m[i].rtyp != INT_CMD) {
            std::cout << "Type mismatch at index " << i << ": l1=" << l1->m[i].rtyp << ", l2=" << l2->m[i].rtyp << std::endl;
            continue;
        }
        
        int v1 = (int)(long)l1->m[i].data;
        int v2 = (int)(long)l2->m[i].data;
        
        std::cout << "Comparing at index " << i << ": " << v1 << " vs " << v2 << std::endl;
        
        if (v1 < v2) return -1;
        if (v1 > v2) return 1;
    }
    
    // If we get here, all compared elements were equal
    // Return -1 if l1 is shorter, 1 if l2 is shorter, 0 if equal length
    if (size1 < size2) return -1;
    if (size1 > size2) return 1;
    return 0;
}

// Lexicographic sort (lexSort)
lists lexSort(lists L) {
    if (!L) return NULL;
    int n = L->nr + 1;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (L->m[j].rtyp != LIST_CMD || L->m[j+1].rtyp != LIST_CMD) continue;
            lists Lj = (lists)L->m[j].data;
            lists Lj1 = (lists)L->m[j+1].data;
            if (!Lj || !Lj1 || Lj->nr < 1 || Lj1->nr < 1) continue;
            
            lists Lj_2 = (lists)Lj->m[1].data;  // Second element of Lj
            lists Lj1_2 = (lists)Lj1->m[1].data; // Second element of Lj1
            
            if (comp_lex(Lj_2, Lj1_2) == 1) {
                // Swap L[j] and L[j+1]
                void* temp = L->m[j].data;
                L->m[j].data = L->m[j+1].data;
                L->m[j+1].data = temp;
            }
        }
    }
    return L;
}

// Substitute parameters in a polynomial (substituteList)
poly substituteList(poly f, lists l, ring R) {
    if (!f || !l || !R) return NULL;
    poly result = p_Copy(f, R);
    
    // For each parameter in the list
    for (int i = 0; i <= l->nr; i++) {
        if (l->m[i].rtyp != NUMBER_CMD) continue;
        number val = (number)l->m[i].data;
        
        // Create a polynomial from the number
        poly val_poly = p_NSet(n_Copy(val, R->cf), R);
        
        // Substitute the value for parameter i+1 (Singular uses 1-based indexing)
        result = p_Subst(result, i + 1, val_poly, R);
        
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

    // Initialize output list
    lists L = (lists)omAlloc0Bin(slists_bin);
    if (!L) {
        WerrorS("Memory allocation failed for output list");
        return NULL;
    }
    L->Init(s);

    // Initialize FLINT random state and modulus
    flint_rand_t state;
    fmpz_t m;
    flint_randinit(state);
    fmpz_init_set_ui(m, p);

    // Generate random integers between 0 and p-1
    for (int i = 0; i < s; i++) {
        // Generate a random number between 0 and p-1 using FLINT
        fmpz_t val;
        fmpz_init(val);
        fmpz_randm(val, state, m);
    
        
        L->m[i].rtyp = INT_CMD  ;
        L->m[i].data = (void*)(long)fmpz_get_si(val);
        
        // Clean up FLINT integer
        fmpz_clear(val);
    }
    std::cout << "L: " << listToString(L) << std::endl;

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

// Helper function to convert Singular number to FLINT rational
void convertSingularToFLINT(fmpq_t result, number n, ring R) {
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
number convertFLINTToSingular(const fmpq_t q, ring R) {
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
lists extractCoef(oneIBP I, lists ind, lists l, ring R) {
    std::cout << "starting extractCoef" << std::endl;
    if (!I || !ind || !l || !R) return NULL;
    
    lists v = (lists)omAlloc0Bin(slists_bin);
    v->Init(ind->nr + 1);
    
    for (int j = 0; j <= ind->nr; j++) {
        int tem = 0;
        lists ind_list = (lists)ind->m[j].data;
        if (!ind_list) continue;
        
        for (int k = 0; k <= I->i->nr; k++) {
            lists ibp_indices = (lists)I->i->m[k].data;
            if (!ibp_indices) continue;
            std::cout<<"comp_lex(ind_list,ibp_indices): "<<comp_lex(ind_list,ibp_indices)<<std::endl;
            if (comp_lex(ind_list,ibp_indices) == 0) {
                // The coefficient is a number in ring R
                if (I->c->m[k].rtyp == NUMBER_CMD) {
                    number n = (number)I->c->m[k].data;
                    // Create a polynomial from the number
                    poly p = p_NSet(n_Copy(n, R->cf), R);
                    if (p) {
                        // Substitute values from l into the polynomial
                        poly result = substituteList(p, l, R);
                        v->m[j].rtyp = POLY_CMD;
                        v->m[j].data = result;
                        tem = 1;
                        p_Delete(&p, R);
                    }
                }
                break;
            }
        }
        
        if (tem == 0) {
            v->m[j].rtyp = NUMBER_CMD;
            v->m[j].data = n_Init(0, R->cf);
        }
    }
    
    return v;
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
    std::cout << "starting setMat" << std::endl;
    
    // Check if we're working with a field of characteristic 0 (Q)
    int charac = n_GetChar(R->cf);
    std::cout << "charac: " << charac << std::endl;
    
    // Count the number of IBP relations
    int count = 0;
    while (S->IBP[count] != NULL) {
        count++;
    }
    
    // Count the number of indices
    int ind_count = 0;
    while (ind->m[ind_count].rtyp == LIST_CMD && ind->m[ind_count].data) {
        ind_count++;
    }
    
    std::cout << "count: " << count << ", ind_count: " << ind_count << std::endl;
    
    // Create a matrix to store the result
    matrix result = mpNew(count, ind_count);
    
    std::cout<<"debugging S"<<std::endl;
    std::cout<<"S->IBP[0]->i: "<<listToString((lists)S->IBP[0]->i)<<std::endl;
    std::cout<<"S->IBP[0]->i->m[0].data: "<<listToString((lists)S->IBP[0]->i->m[0].data)<<std::endl;
    std::cout<<"S->IBP[0]->c: "<<listToString((lists)S->IBP[0]->c)<<std::endl;
    std::cout<<" type of S->IBP[0]->c->m[0].data: "<<S->IBP[0]->c->m[0].Typ()<<std::endl;
    std::cout<<"S->IBP[0]->c->m[0].data: "<<numberToString((number)(S->IBP[0]->c->m[0].data),R)<<std::endl;
    // For each IBP relation
    for (int i = 0; i < count; i++) {
        // Extract coefficients for this IBP
        lists coef = extractCoef(S->IBP[i], ind, val, R);
        std::cout << "coef: " << listToString(coef) << std::endl;
        if (!coef) {
            // Handle error
            mp_Delete(&result, R);
        return NULL;
    }

        std::cout << "coef: " << listToString(coef) << std::endl;
        
        // Fill the matrix row
        for (int j = 0; j < ind_count; j++) {
            if (coef->m[j].rtyp == NUMBER_CMD) {
                number n = (number)coef->m[j].data;
                MATELEM(result, i+1, j+1) = p_NSet(n_Copy(n, R->cf), R);
            } else if (coef->m[j].rtyp == INT_CMD) {
                // Convert integer to number
                number n = n_Init((int)(long)coef->m[j].data, R->cf);
                MATELEM(result, i+1, j+1) = p_NSet(n, R);
            } else {
                MATELEM(result, i+1, j+1) = p_NSet(n_Init(0, R->cf), R);
            }
        }
        
        // Clean up the coefficient list
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
    std::cout << "setMat" << std::endl;
   
    
    lists input_S = (lists)args->Data();
  
    lists Set = (lists)input_S->m[3].data;
    
    ring R = (ring)Set->m[0].data;
    
    std::cout << "R: " << rString(R) << std::endl;
    
    if (!args->next) {
        WerrorS("Second argument (input_ind) is NULL");
        return NULL;
    }
    lists input_ind = (lists)(args->next->Data());
  
    

    int p = (int)(long)(args->next->next->Data());
    
    std::cout << "p: " << p << std::endl;
    
    lists ibp = (lists)Set->m[3].data;
    if (!ibp) {
        WerrorS("Invalid ibp list");
        return NULL;
    }

    lists ind = (lists)input_ind->m[3].data;
    if (!ind) {
        WerrorS("Invalid ind list");
        return NULL;
    }
    
    std::cout << "ibp->nr: " << ibp->nr << std::endl;
    
    // Create setIBP structure
    setIBP S = (setIBP)omAlloc0(sizeof(*S));
    if (!S) {
        WerrorS("Memory allocation failed for setIBP");
        return NULL;
    }
    
    // Allocate IBP array
    S->IBP = (oneIBP*)omAlloc0(sizeof(oneIBP) * (ibp->nr + 2));
    if (!S->IBP) {
        omFreeSize(S, sizeof(*S));
        WerrorS("Memory allocation failed for IBP array");
            return NULL;
        }

    // Copy IBP data
    for (int i = 0; i <= ibp->nr; i++) {
        //std::cout << "Processing IBP item " << i << std::endl;
        if (ibp->m[i].rtyp == LIST_CMD && ibp->m[i].data) {
            lists ibp_item = (lists)ibp->m[i].data;
            //std::cout << "IBP item " << i << " has " << ibp_item->nr + 1 << " elements" << std::endl;
            
            S->IBP[i] = (oneIBP)omAlloc0(sizeof(*S->IBP[i]));
          

            if (ibp_item->nr >= 1) {
                // Check coefficient list
                if (ibp_item->m[0].rtyp == LIST_CMD && ibp_item->m[0].data) {
                    S->IBP[i]->c = (lists)ibp_item->m[0].data;
                    //std::cout << "Coefficient list for IBP " << i << " has " << S->IBP[i]->c->nr + 1 << " elements" << std::endl;
                }
                // Check index list
                if (ibp_item->m[1].rtyp == LIST_CMD && ibp_item->m[1].data) {
                    S->IBP[i]->i = (lists)ibp_item->m[1].data;
                    //std::cout << "Index list for IBP " << i << " has " << S->IBP[i]->i->nr + 1 << " elements" << std::endl;
                }
            }
        }
    }
    S->IBP[ibp->nr + 1] = NULL; // Ensure NULL terminator
    S->over = R;
    
    std::cout << "R: " << rString(R) << std::endl;
    int n = rVar(R);
    std::cout << "n: " << n << std::endl;
    
    // Make sure we're in the correct ring
    ring savedRing = currRing;
    rChangeCurrRing(R);
    
    lists input_val = getRandom(p, n);
    std::cout << "input_val: " << listToString(input_val) << std::endl;
    
    if (!input_val) {
        // Clean up
        for (int i = 0; i <= ibp->nr; i++) {
            if (S->IBP[i]) omFreeSize(S->IBP[i], sizeof(*S->IBP[i]));
        }
        omFreeSize(S->IBP, sizeof(oneIBP) * (ibp->nr + 2));
        omFreeSize(S, sizeof(*S));
        rChangeCurrRing(savedRing);
        WerrorS("Failed to generate random values");
        return NULL;
    }
    std::cout<<"starting setMat"<<std::endl;
    matrix result = setMat(S, input_val, ind, R);
    
    // Clean up
    for (int i = 0; i <= ibp->nr; i++) {
        if (S->IBP[i]) omFreeSize(S->IBP[i], sizeof(*S->IBP[i]));
    }
    omFreeSize(S->IBP, sizeof(oneIBP) * (ibp->nr + 2));
    omFreeSize(S, sizeof(*S));
    
    // Clean up input_val
    for (int i = 0; i <= input_val->nr; i++) {
        if (input_val->m[i].rtyp == NUMBER_CMD) {
            n_Delete((number*)&input_val->m[i].data, R->cf);
        }
    }
    omFreeBin(input_val, slists_bin);
    
    // Restore the original ring
    rChangeCurrRing(savedRing);
    
    lists output = (lists)omAlloc0Bin(slists_bin);
    output->Init(1);
    output->m[0].rtyp = MATRIX_CMD;
    output->m[0].data = result;
    
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
