
// Print Web structure
void printWeb(const Web& web) {
    std::cout << "Web Structure:\n";
    for (size_t i = 0; i < web.sectors.size(); ++i) {
        std::cout << "  Layer " << (i + 1) << ":\n";
        for (size_t j = 0; j < web.sectors[i].size(); ++j) {
            std::cout << "    Sector (" << (i + 1) << "," << (j + 1) << "): Label = {";
            for (size_t k = 0; k < web.sectors[i][j].size(); ++k) {
                std::cout << web.sectors[i][j][k];
                if (k < web.sectors[i][j].size() - 1) std::cout << ", ";
            }
            std::cout << "}\n";
        }
    }
}

// Print GraphData structure
void printGraphData(const GraphData& graph) {
    std::cout << "Graph Data:\n";
    std::cout << "  Vertices: {";
    for (size_t i = 0; i < graph.vertices.size(); ++i) {
        std::cout << graph.vertices[i];
        if (i < graph.vertices.size() - 1) std::cout << ", ";
    }
    std::cout << "}\n";
    std::cout << "  Edges:\n";
    for (const auto& edge : graph.edges) {
        std::cout << "    (" << edge.first << ") -> (" << edge.second << ")\n";
    }
}

// Print DAG structure
void printDAG(const DAG& dag) {
    std::cout << "DAG:\n";
    std::cout << "  Vertices: {";
    for (size_t i = 0; i < dag.vertices.size(); ++i) {
        std::cout << dag.vertices[i];
        if (i < dag.vertices.size() - 1) std::cout << ", ";
    }
    std::cout << "}\n";
    std::cout << "  Edges:\n";
    for (const auto& edge : dag.edges) {
        std::cout << "    (" << edge.first << ") -> (" << edge.second << ")\n";
    }
    std::cout << "  Parents:\n";
    for (const auto& [vertex, parents] : dag.parents) {
        std::cout << "    " << vertex << ": {";
        for (size_t i = 0; i < parents.size(); ++i) {
            std::cout << parents[i];
            if (i < parents.size() - 1) std::cout << ", ";
        }
        std::cout << "}\n";
    }
}

// Print PetriNet structure
void printPetriNet(const PetriNet& net) {
    std::cout << "Petri Net:\n";
    std::cout << "  Places:\n";
    for (const auto& place : net.places) {
        auto it = net.place_types.find(place);
        std::string type = (it != net.place_types.end()) ? it->second : "unknown";
        std::cout << "    " << place << " (type: " << type << ")\n";
    }
    std::cout << "  Transitions:\n";
    for (const auto& transition : net.transitions) {
        std::cout << "    " << transition << ":\n";
        if (net.transition_inputs.count(transition)) {
            std::cout << "      Inputs: {";
            for (size_t i = 0; i < net.transition_inputs.at(transition).size(); ++i) {
                std::cout << net.transition_inputs.at(transition)[i];
                if (i < net.transition_inputs.at(transition).size() - 1) std::cout << ", ";
            }
            std::cout << "}\n";
        }
        if (net.transition_outputs.count(transition)) {
            std::cout << "      Outputs: {";
            for (size_t i = 0; i < net.transition_outputs.at(transition).size(); ++i) {
                std::cout << net.transition_outputs.at(transition)[i];
                if (i < net.transition_outputs.at(transition).size() - 1) std::cout << ", ";
            }
            std::cout << "}\n";
        }
    }
}

// Mock generateWeb to simulate web_g
Web generateWeb(const std::vector<std::vector<int>>& targetInt) {
    Web web;
    // Simulate web structure based on example
    // For targetInt = {{1,1,1,-1,-3,-1,-1,-1,-1}, {1,-1,1,-1,-3,-1,-1,-4,-1}}
    web.sectors = {
        {{{1,2,3}}}, // Layer 1: sector (1,1) with label {1,2,3}
        {{{1,2}, {2,3}, {1,3}}}, // Layer 2: sectors (2,1), (2,2), (2,3)
        {{{1}, {2}, {3}}} // Layer 3: sectors (3,1), (3,2), (3,3)
    };
    return web;
}

// Simulate getGraph to generate vertices and edges
GraphData getGraph(const Web& web) {
    GraphData graph;
    std::map<std::pair<int, int>, std::vector<int>> labels;

    // Generate vertices
    for (size_t i = 0; i < web.sectors.size(); ++i) {
        for (size_t j = 0; j < web.sectors[i].size(); ++j) {
            std::string vertex = std::to_string(i + 1) + std::to_string(j + 1);
            graph.vertices.push_back(vertex);
            labels[{i + 1, j + 1}] = web.sectors[i][j];
        }
    }

    // Generate edges based on subset relationships
    for (size_t i = 0; i < web.sectors.size() - 1; ++i) {
        for (size_t j = 0; j < web.sectors[i].size(); ++j) {
            for (size_t k = 0; k < web.sectors[i + 1].size(); ++k) {
                std::vector<int> small = web.sectors[i + 1][k];
                std::vector<int> big = web.sectors[i][j];
                bool is_subset = true;
                for (int x : small) {
                    if (std::find(big.begin(), big.end(), x) == big.end()) {
                        is_subset = false;
                        break;
                    }
                }
                if (is_subset) {
                    std::string from = std::to_string(i + 1) + std::to_string(j + 1);
                    std::string to = std::to_string(i + 2) + std::to_string(k + 1);
                    graph.edges.emplace_back(from, to);
                }
            }
        }
    }

    return graph;
}
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

// Define oneSector structure
struct oneSector {
    std::vector<int> lab;
    std::vector<int> sectorMap;
    std::vector<int> targetInts;
};

// Helper function to lexicographically sort a vector of vectors
std::vector<std::vector<std::vector<int>>> lexSort(const std::vector<std::vector<std::vector<int>>>& vec) {
    std::vector<std::vector<std::vector<int>>> sorted = vec;
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        // Compare based on first element (L[i][1][1] in original)
        return a[0] < b[0];
    });
    return sorted;
}

// Helper function to delete an element from a vector at index (1-based in original)
std::vector<int> deleteElement(const std::vector<int>& vec, int index) {
    std::vector<int> result = vec;
    if (index >= 1 && index <= static_cast<int>(vec.size())) {
        result.erase(result.begin() + (index - 1));
    }
    return result;
}

// getSector: Returns sector (binary vector) and indices of non-zero entries
std::vector<std::vector<int>> getSector(const std::vector<int>& l) {
    std::vector<int> s(l.size(), 0);
    std::vector<int> n;

    // 1-based indexing adjusted to 0-based
    for (size_t i = 0; i < l.size(); ++i) {
        if (l[i] > 0) {
            s[i] = 1;
            n.push_back(i + 1); // Store 1-based index as per original
        }
    }

    return {s, n};
}

// isSubList: Checks if l1 is a sublist of l2
int isSubList(const std::vector<int>& l1, const std::vector<int>& l2) {
    if (l1.size() > l2.size()) {
        return -1;
    }

    int ind = 0;
    for (size_t i = 0; i < l1.size(); ++i) {
        for (size_t j = 0; j < l2.size(); ++j) {
            if (l2[j] == l1[i]) {
                ind++;
                break; // Found match, move to next element in l1
            }
        }
    }

    return (ind == static_cast<int>(l1.size())) ? 1 : 0;
}

// pickHighestSector: Identifies highest sector integrals
std::vector<std::vector<std::vector<int>>> pickHighestSector(const std::vector<std::vector<int>>& targetInt) {
    std::vector<std::vector<std::vector<int>>> sortedInt1; // List of sector labels
    std::vector<std::vector<std::vector<int>>> sortedInt2; // List of integral collections
    std::vector<std::vector<std::vector<int>>> intsWithSectors;

    // Build intsWithSectors
    for (size_t i = 0; i < targetInt.size(); ++i) {
        auto L = getSector(targetInt[i]);
        // L[0] = sector (s), L[1] = indices (n)
        std::vector<std::vector<int>> entry = {{L[1], targetInt[i]}, {static_cast<int>(L[1].size())}};
        intsWithSectors.push_back(entry);
    }

    // Lexicographically sort (reverse order to match original)
    auto L = lexSort(intsWithSectors);
    std::reverse(L.begin(), L.end());

    // Process sorted sectors
    for (size_t i = 0; i < L.size(); ++i) {
        if (sortedInt1.empty()) {
            sortedInt1.push_back({L[i][0][0]}); // Sector label
            sortedInt2.push_back({L[i][0][1]}); // Integral
        } else {
            int id = 0;
            for (size_t j = 0; j < sortedInt1.size(); ++j) {
                if (isSubList(L[i][0][0], sortedInt1[j][0]) == 1) {
                    sortedInt2[j].push_back(L[i][0][1]);
                    id++;
                }
            }
            if (id == 0) {
                sortedInt1.push_back({L[i][0][0]});
                sortedInt2.push_back({L[i][0][1]});
            }
        }
    }

    return sortedInt2;
}

// getCombinations: Recursive helper for combinations
std::vector<std::vector<int>> getCombinations(const std::vector<int>& L, int r, const std::vector<int>& pre) {
    std::vector<std::vector<int>> op;
    int n = static_cast<int>(L.size());

    if (static_cast<int>(pre.size()) == r) {
        op.push_back(pre);
        return op;
    }
    if (n == 0) {
        return {};
    }

    std::vector<int> remL = deleteElement(L, 1); // Delete first element (1-based index)
    std::vector<int> npre = pre;
    npre.push_back(L[0]);
    auto ex1 = getCombinations(remL, r, npre);
    auto ex2 = getCombinations(remL, r, pre);

    // Concatenate results
    op.insert(op.end(), ex1.begin(), ex1.end());
    op.insert(op.end(), ex2.begin(), ex2.end());
    return op;
}

// listCombinations: Generates r-combinations of list L
std::vector<std::vector<int>> listCombinations(const std::vector<int>& L, int r) {
    return getCombinations(L, r, {});
}

// generateWebSectors: Generates web structure from seed
std::vector<std::vector<oneSector>> generateWebSectors(const std::vector<int>& seed) {
    auto L = getSector(seed);
    std::vector<int> sector = L[0]; // Binary sector
    std::vector<int> indices = L[1]; // Non-zero indices
    int nSectors = static_cast<int>(std::pow(2, indices.size()));
    std::vector<std::vector<oneSector>> sectorWeb(indices.size() + 1); // +1 for 0-based adjustment

    for (size_t i = 0; i <= indices.size(); ++i) {
        auto comb = listCombinations(indices, indices.size() - i);
        std::vector<oneSector> layer;
        for (const auto& c : comb) {
            oneSector s;
            s.lab = c;
            // sectorMap and targetInts left empty as per original
            layer.push_back(s);
        }
        sectorWeb[i] = layer;
    }

    return sectorWeb;
}

// getweb: Generates web from targetInt
std::vector<std::vector<std::vector<int>>> getweb(const std::vector<std::vector<int>>& targetInt) {
    auto L = pickHighestSector(targetInt);
    if (L.empty() || L[0].empty()) {
        return {};
    }

    // Generate web from first integral of first collection
    auto web = generateWebSectors(L[0][0]);

    // Convert oneSector web to vector-based web for compatibility
    std::vector<std::vector<std::vector<int>>> result(web.size());
    for (size_t i = 0; i < web.size(); ++i) {
        result[i].resize(web[i].size());
        for (size_t j = 0; j < web[i].size(); ++j) {
            result[i][j] = web[i][j].lab;
        }
    }

    return result;
}

#endif
