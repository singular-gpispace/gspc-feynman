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
