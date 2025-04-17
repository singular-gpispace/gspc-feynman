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



// Print an fq_mat_t matrix for debugging
void print_fq_mat(const char* label, const fq_mat_t mat, const fq_ctx_t ctx) {
    std::cout << label << "\n-------------\n";
    for (slong i = 0; i < fq_mat_nrows(mat, ctx); i++) {
        for (slong j = 0; j < fq_mat_ncols(mat, ctx); j++) {
            fq_print_pretty(fq_mat_entry(mat, i, j), ctx);
            std::cout << "  ";
        }
        std::cout << "\n";
    }
    std::cout << "-------------\n";
}

// Manually set a matrix to the identity matrix
void fq_mat_set_one(fq_mat_t mat, const fq_ctx_t ctx) {
    fq_mat_zero(mat, ctx);
    slong n = fq_mat_nrows(mat, ctx);
    slong m = fq_mat_ncols(mat, ctx);
    slong min = (n < m) ? n : m;
    fq_t one;
    fq_init(one, ctx);
    fq_one(one, ctx);
    for (slong i = 0; i < min; i++) {
        fq_mat_entry_set(mat, i, i, one, ctx);
    }
    fq_clear(one, ctx);
}

// Structure to hold the result (P, U, S, rank)
typedef struct {
    fq_nmod_mat_t P;  // Permutation matrix
    fq_nmod_mat_t U;  // Upper triangular matrix
    fq_nmod_mat_t S;  // Row echelon form of A
    slong rank;  // Rank of the matrix
} gaussred_result_t;

// Function declarations
static void cleanupList(lists L);
static gaussred_result_t gaussred_fq_nmod(const fq_nmod_mat_t A, const fq_nmod_ctx_t ctx);

// Function to clear the result structure
static void gaussred_result_clear(gaussred_result_t* result, const fq_nmod_ctx_t ctx) {
    if (result) {
        fq_nmod_mat_clear(result->P, ctx);
        fq_nmod_mat_clear(result->U, ctx);
        fq_nmod_mat_clear(result->S, ctx);
    }
}



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

// Set a matrix to be a copy of another matrix
extern void fq_nmod_mat_set(fq_nmod_mat_t dest, const fq_nmod_mat_t src, const fq_nmod_ctx_t ctx);

// Helper function to get unsigned integer value from fq_nmod element
static mp_limb_t fq_nmod_get_ui(const fq_nmod_t x, const fq_nmod_ctx_t ctx) {
    (void)ctx;  // Mark ctx as used to avoid warning
    return x->coeffs[0];  // For degree 1 fields, the value is in the first coefficient
}

lists singflint_rref_full(matrix m, const ring R, int p)
{
    if (!m || !R) {
        WerrorS("NULL matrix or ring");
        return NULL;
    }

    int r = m->rows();
    int c = m->cols();
    std::cout << "r=" << r << std::endl;
    std::cout << "c=" << c << std::endl;
    std::cout << "R=" << rString(R) << std::endl;
    std::cout << "Coefficient field type: " << nCoeffString(R->cf) << std::endl;

std::cout<<"char(R)="<<rChar(R)<<std::endl;
    matrix M = NULL;
    matrix P = NULL;
    matrix U = NULL;
    int rank = 0;
    
    ring savedRing = currRing;
    if (savedRing != R) {
        rChangeCurrRing(R);
    }

  
        std::cout<<"rField_is_Zp(R)"<<std::endl;
        // Initialize FLINT finite field context for Z/p
        fmpz_t pp;
        fmpz_init_set_ui(pp,p);
        fq_nmod_ctx_t ctx;
        fq_nmod_ctx_init(ctx, pp, 1, "x"); // Degree 1 for Z/p

        // Convert SINGULAR matrix to fq_nmod_mat_t
        fq_nmod_mat_t A;
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
                        WerrorS("matrix entry is not constant");
                        fq_nmod_mat_clear(A, ctx);
                        fq_nmod_ctx_clear(ctx);
                        fmpz_clear(pp);
                        if (savedRing != R) rChangeCurrRing(savedRing);
                        return NULL;
                    }
                }
            }
        }

        // Compute RREF using gaussred_fq_nmod
        gaussred_result_t result = gaussred_fq_nmod(A, ctx);
        rank = result.rank;
        std::cout << "Rank: " << rank << std::endl;
        
        // Print the reduced matrix for debugging

        // Convert result.S back to SINGULAR matrix
        M = mpNew(r, c);
        for (int i = 1; i <= r; i++) {
            for (int j = 1; j <= c; j++) {
                fq_nmod_t entry;
                fq_nmod_init(entry, ctx);
                fq_nmod_set(entry, fq_nmod_mat_entry(result.S, i-1, j-1), ctx);
                if (!fq_nmod_is_zero(entry, ctx)) {
                    mp_limb_t val = fq_nmod_get_ui(entry, ctx);
                    number n = n_Init(val, R->cf);
                    MATELEM(M, i, j) = p_NSet(n, R);
                }
                fq_nmod_clear(entry, ctx);
            }
        }
        
        // Convert result.P back to SINGULAR matrix
        P = mpNew(r, r);
        for (int i = 1; i <= r; i++) {
            for (int j = 1; j <= r; j++) {
                fq_nmod_t entry;
                fq_nmod_init(entry, ctx);
                fq_nmod_set(entry, fq_nmod_mat_entry(result.P, i-1, j-1), ctx);
                if (!fq_nmod_is_zero(entry, ctx)) {
                    mp_limb_t val = fq_nmod_get_ui(entry, ctx);
                    number n = n_Init(val, R->cf);
                    MATELEM(P, i, j) = p_NSet(n, R);
                }
                fq_nmod_clear(entry, ctx);
            }
        }
        
        // Convert result.U back to SINGULAR matrix
        U = mpNew(r, r);
        for (int i = 1; i <= r; i++) {
            for (int j = 1; j <= r; j++) {
                fq_nmod_t entry;
                fq_nmod_init(entry, ctx);
                fq_nmod_set(entry, fq_nmod_mat_entry(result.U, i-1, j-1), ctx);
                if (!fq_nmod_is_zero(entry, ctx)) {
                    mp_limb_t val = fq_nmod_get_ui(entry, ctx);
                    number n = n_Init(val, R->cf);
                    MATELEM(U, i, j) = p_NSet(n, R);
                }
                fq_nmod_clear(entry, ctx);
            }
        }
        
        std::cout << "M=" << std::endl;
        std::cout << "size of M=" << MATROWS(M) << "x" << MATCOLS(M) << std::endl;
        
        // Clean up
        gaussred_result_clear(&result, ctx);
        fq_nmod_mat_clear(A, ctx);
        fq_nmod_ctx_clear(ctx);
        fmpz_clear(pp);


    // Create a lists structure to hold all results
    lists L = (lists)omAlloc0Bin(slists_bin);
    L->Init(4);
    
    // Add matrices and rank to the list
    L->m[0].rtyp = MATRIX_CMD;
    L->m[0].data = M;
    
    L->m[1].rtyp = MATRIX_CMD;
    L->m[1].data = P;
    
    L->m[2].rtyp = MATRIX_CMD;
    L->m[2].data = U;
    
    L->m[3].rtyp = INT_CMD;
    L->m[3].data = (void*)(long)rank;

    if (savedRing != R) {
        rChangeCurrRing(savedRing);
    }
    return L;
}

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
    lists Z = singflint_rref_full(A, RZ,p);

    matrix P = (matrix)Z->m[0].Data();
    matrix U = (matrix)Z->m[1].Data();
    matrix M = (matrix)Z->m[2].Data();
    int rank = (int)(long)Z->m[3].Data();
   std::cout<<"printing P"<<std::endl;
   printMat(P);
   std::cout<<"printing U"<<std::endl;
   printMat(U);
   std::cout<<"printing M"<<std::endl;
   printMat(M);
   std::cout<<"size of P="<<MATROWS(P)<<"x"<<MATCOLS(P)<<std::endl;
   std::cout<<"size of U="<<MATROWS(U)<<"x"<<MATCOLS(U)<<std::endl;
   std::cout<<"size of M="<<MATROWS(M)<<"x"<<MATCOLS(M)<<std::endl;
   std::cout<<"rank="<<rank<<std::endl;

    std::cout<<"Current ring="<<rString(currRing)<<std::endl;
std::cout<<"Ring RZ="<<rString(RZ)<<std::endl;

    // Create output list carefully
    lists output = (lists)omAlloc0Bin(slists_bin);

    output->Init(4);

    // Create type list carefully
    lists t1 = (lists)omAlloc0Bin(slists_bin);
  
    t1->Init(2);
    t1->m[0].rtyp = STRING_CMD;
    t1->m[0].data = omStrDup("generators");
    t1->m[1].rtyp = STRING_CMD;
    t1->m[1].data = omStrDup("singflint_rref");

    // Set output fields carefully
    output->m[0].rtyp = RING_CMD;
    output->m[0].data = currRing;
    output->m[1].rtyp = LIST_CMD;
    output->m[1].data = t1;
    output->m[2].rtyp = RING_CMD;
    output->m[2].data = currRing;
    output->m[3].rtyp = LIST_CMD;
    output->m[3].data = lCopy(Z);

    // Clean up original result and restore ring
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

// Helper function to clean up a list and its contents
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
