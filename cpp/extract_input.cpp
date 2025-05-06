#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
#include <numeric>
#include <string>

struct OneSector {
    std::vector<int> lab;
    std::vector<std::vector<int>> sectorMap;
    std::vector<int> targetInts;
};

// Forward declarations
std::vector<std::vector<OneSector>> web_g(const std::vector<std::vector<std::vector<int>>>& targetInt);
std::vector<std::vector<int>> pickHighestSector(const std::vector<std::vector<int>>& targetInt);
void getSector(const std::vector<int>& l, std::vector<int>& s, std::vector<int>& n);
int isSubList(const std::vector<int>& l1, const std::vector<int>& l2);
int comp_lex(const std::vector<int>& l1, const std::vector<int>& l2);
std::vector<std::vector<std::vector<int>>> lexSort(const std::vector<std::vector<std::vector<int>>>& L);
std::vector<std::vector<OneSector>> generateWebSectors(const std::vector<int>& seed);
std::vector<std::vector<int>> listCombinations(const std::vector<int>& L, int r);
std::vector<std::vector<int>> getCombinations(const std::vector<int>& L, int r, const std::vector<int>& pre);
std::vector<int> getlabels(const std::vector<std::vector<OneSector>>& web, int i, int j);
void print_graph(const std::vector<std::vector<std::vector<int>>>& graph);
std::vector<std::vector<OneSector>> generateWebStructure(const std::vector<std::vector<int>>& input_sectors);
void generateCombinations(const std::vector<int>& elements, size_t r, std::vector<std::vector<int>>& result);
void generateCombinationsHelper(const std::vector<int>& elements, size_t r, size_t start, 
                              std::vector<int>& combination, std::vector<std::vector<int>>& result);
void generateGraph(const std::vector<int>& positive_indices);
bool isSubset(const std::vector<int>& small, const std::vector<int>& big);

std::vector<std::vector<OneSector>> web_g(const std::vector<std::vector<std::vector<int>>>& targetInt) {
    std::cout << "Processing input sectors:\n";
    for (size_t i = 0; i < targetInt[0].size(); ++i) {
        std::cout << "Sector " << i + 1 << ": ";
        for (int val : targetInt[0][i]) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }

    std::vector<std::vector<int>> L = pickHighestSector(targetInt[0]);
    std::cout << "L[0].size(): " << L[0].size() << "\n";
    std::cout << "Highest sector indices: ";
    for (int idx : L[0]) {
        std::cout << idx << " ";
    }
    std::cout << "\n";

    std::vector<int> s, n;
    getSector(L[0], s, n);
    std::cout << "Positive number indices: ";
    for (int idx : n) {
        std::cout << idx << " ";
    }
    std::cout << "\n";

    return generateWebSectors(n);
}

std::vector<std::vector<int>> pickHighestSector(const std::vector<std::vector<int>>& targetInt) {
    std::vector<std::vector<std::vector<int>>> intsWithSectors;
    
    // Process each sector
    for (size_t i = 0; i < targetInt.size(); ++i) {
        std::vector<int> s, n;
        getSector(targetInt[i], s, n);
        
        // Create entry with sector info and original values
        std::vector<std::vector<int>> entry;
        entry.push_back(s);  // sector pattern
        entry.push_back(targetInt[i]);  // original values
        intsWithSectors.push_back(entry);
    }

    // Sort sectors
    std::vector<std::vector<std::vector<int>>> L = lexSort(intsWithSectors);
    std::vector<std::vector<int>> sortedInt1;
    std::vector<std::vector<int>> sortedInt2;

    // Process sorted sectors
    for (int i = L.size() - 1; i >= 0; --i) {
        if (sortedInt1.empty()) {
            sortedInt1.push_back(L[i][0]);
            sortedInt2.push_back(L[i][1]);
        } else {
            int id = 0;
            for (size_t j = 0; j < sortedInt1.size(); ++j) {
                if (isSubList(L[i][0], sortedInt1[j]) == 1) {
                    sortedInt2[j].push_back(L[i][1][0]);
                    id++;
                }
            }
            if (id == 0) {
                sortedInt1.push_back(L[i][0]);
                sortedInt2.push_back(L[i][1]);
            }
        }
    }

    // Debug output
    std::cout << "Sorted sectors:\n";
    for (size_t i = 0; i < sortedInt1.size(); ++i) {
        std::cout << "Sector " << i + 1 << " pattern: ";
        for (int val : sortedInt1[i]) std::cout << val << " ";
        std::cout << "\nValues: ";
        for (int val : sortedInt2[i]) std::cout << val << " ";
        std::cout << "\n";
    }

    return sortedInt2;
}

void getSector(const std::vector<int>& l, std::vector<int>& s, std::vector<int>& n) {
    std::cout << "in getSector, l.size(): " << l.size() << std::endl;
    s.clear();
    n.clear();
    
    // Initialize s with zeros for all positions
    s.resize(l.size(), 0);
    
    // Find positive indices and set corresponding positions in s to 1
    for (size_t i = 0; i < l.size(); i++) {
        if (l[i] > 0) {
            s[i] = 1;
            n.push_back(i + 1);  // Using 1-based indexing like Singular
        }
    }
    
    std::cout << "in getSector, s: ";
    for (size_t i = 0; i < s.size(); i++) {
        std::cout << s[i];
        if (i < s.size() - 1) std::cout << ",";
    }
    std::cout << std::endl;
    
    std::cout << "in getSector, n: ";
    for (size_t i = 0; i < n.size(); i++) {
        std::cout << n[i];
        if (i < n.size() - 1) std::cout << ",";
    }
    std::cout << std::endl;
}

int isSubList(const std::vector<int>& l1, const std::vector<int>& l2) {
    if (l1.size() > l2.size()) {
        return -1;
    }
    int ind = 0;
    for (size_t i = 0; i < l1.size(); ++i) {
        for (size_t j = 0; j < l2.size(); ++j) {
            if (l2[j] == l1[i]) {
                ind++;
                break;
            }
        }
    }
    return (ind == static_cast<int>(l1.size())) ? 1 : 0;
}

int comp_lex(const std::vector<int>& l1, const std::vector<int>& l2) {
    size_t min_size = std::min(l1.size(), l2.size());
    for (size_t i = 0; i < min_size; ++i) {
        if (l1[i] < l2[i]) return -1;
        if (l1[i] > l2[i]) return 1;
    }
    return 0;
}

std::vector<std::vector<std::vector<int>>> lexSort(const std::vector<std::vector<std::vector<int>>>& L) {
    std::vector<std::vector<std::vector<int>>> result = L;
    size_t n = result.size();
    for (size_t i = 0; i < n - 1; ++i) {
        for (size_t j = 0; j < n - i - 1; ++j) {
            if (comp_lex(result[j][1], result[j + 1][1]) == 1) {
                std::swap(result[j], result[j + 1]);
            }
        }
    }
    return result;
}

std::vector<std::vector<OneSector>> generateWebSectors(const std::vector<int>& seed) {
    std::vector<std::vector<OneSector>> web;
    
    // Create layers based on the size of positive numbers
    for (size_t i = 0; i < seed.size(); ++i) {
        std::vector<OneSector> layer;
        // Get combinations of size seed.size() - i
        std::vector<std::vector<int>> combinations = listCombinations(seed, seed.size() - i);
        
        // Create sectors for this layer
        for (const auto& comb : combinations) {
            OneSector newSector;
            newSector.lab = comb;
            newSector.sectorMap = {};
            newSector.targetInts = {};
            layer.push_back(newSector);
        }
        web.push_back(layer);
    }
    
    return web;
}

std::vector<std::vector<int>> listCombinations(const std::vector<int>& L, int r) {
    return getCombinations(L, r, {});
}

std::vector<std::vector<int>> getCombinations(const std::vector<int>& L, int r, const std::vector<int>& pre) {
    std::vector<std::vector<int>> op;
    if (pre.size() == static_cast<size_t>(r)) {
        op.push_back(pre);
        return op;
    }
    if (L.empty()) {
        return {};
    }
    std::vector<int> remL = L;
    remL.erase(remL.begin());
    std::vector<int> npre = pre;
    npre.push_back(L[0]);
    std::vector<std::vector<int>> ex1 = getCombinations(remL, r, npre);
    std::vector<std::vector<int>> ex2 = getCombinations(remL, r, pre);
    op.insert(op.end(), ex1.begin(), ex1.end());
    op.insert(op.end(), ex2.begin(), ex2.end());
    return op;
}

std::vector<int> getlabels(const std::vector<std::vector<OneSector>>& web, int i, int j) {
    return web[i-1][j-1].lab;
}

void print_web(const std::vector<std::vector<OneSector>>& web) {
    std::cout << " web is :\n";
    if (web.empty()) {
        std::cout << "  (empty list)\n";
        return;
    }

    for (size_t i = 0; i < web.size(); ++i) {
        std::cout << "  Layer " << (i + 1) << ":\n";
        for (size_t j = 0; j < web[i].size(); ++j) {
            const auto& sector = web[i][j];
            std::cout << "    Sector " << (j + 1) << ":\n";
            
            // Print lab with values
            std::cout << "      lab = ";
            for (size_t k = 0; k < sector.lab.size(); ++k) {
                std::cout << sector.lab[k];
                if (k < sector.lab.size() - 1) std::cout << ",";
            }
            std::cout << "\n";
            
            // Print sectorMap
            std::cout << "      sectorMap = ";
            if (sector.sectorMap.empty()) {
                std::cout << "\n";
            } else {
                for (size_t k = 0; k < sector.sectorMap.size(); ++k) {
                    std::cout << sector.sectorMap[k][0];
                    if (k < sector.sectorMap.size() - 1) std::cout << ",";
                }
                std::cout << "\n";
            }
            
            // Print targetInts
            std::cout << "      targetInts:\n";
            if (sector.targetInts.empty()) {
                std::cout << "        (empty)\n";
            } else {
                for (size_t k = 0; k < sector.targetInts.size(); ++k) {
                    std::cout << "        [" << (k + 1) << "] = " << sector.targetInts[k] << "\n";
                }
            }
        }
    }
}

// Helper function to check if list contains element
bool contains(const std::vector<int>& L, int x) {
    return std::find(L.begin(), L.end(), x) != L.end();
}

// Helper function to determine subset
bool subset(const std::vector<int>& small, const std::vector<int>& big) {
    for (int x : small) {
        if (!contains(big, x)) {
            return false;
        }
    }
    return true;
}

// Helper function to convert vector to string for printing
std::string vectorToString(const std::vector<int>& vec) {
    std::stringstream ss;
    ss << "{";
    for (size_t i = 0; i < vec.size(); ++i) {
        ss << vec[i];
        if (i < vec.size() - 1) ss << ",";
    }
    ss << "}";
    return ss.str();
}

std::vector<std::vector<std::vector<int>>> getGraph(const std::vector<std::vector<OneSector>>& T) {
    std::vector<std::vector<int>> vertices;
    std::vector<std::vector<int>> edges;  // Changed to 2D vector for edges
    std::vector<std::vector<int>> labels;
    int v_idx = 0; // 0-based indexing
    int e_idx = 0;

    std::cout << "size of web T: " << T.size() << "\n";

    // Generate vertices and labels
    for (size_t i = 0; i < T.size(); ++i) {
        std::cout << "size of T[" << (i + 1) << "]: " << T[i].size() << "\n";
        for (size_t j = 0; j < T[i].size(); ++j) {
            vertices.push_back({static_cast<int>(i + 1), static_cast<int>(j + 1)});
            labels.push_back(T[i][j].lab);
            v_idx++;
        }
    }

    std::cout << "Vertices:\n";
    for (size_t i = 0; i < vertices.size(); ++i) {
        std::cout << "(" << vertices[i][0] << ", " << vertices[i][1] << ")    ("
                  << vectorToString(labels[i]) << ")\n";
    }

    // Generate edges
    for (size_t i = 0; i < T.size() - 1; ++i) {
        for (size_t j = 0; j < T[i].size(); ++j) {
            for (size_t k = 0; k < T[i + 1].size(); ++k) {
                if (subset(T[i + 1][k].lab, T[i][j].lab)) {
                    // Store edges as flattened pairs: {i1,j1, i2,j2}
                    edges.push_back({static_cast<int>(i + 1), static_cast<int>(j + 1),
                                   static_cast<int>(i + 2), static_cast<int>(k + 1)});
                    e_idx++;
                }
            }
        }
    }

    std::cout << "Edges:\n";
    for (const auto& edge : edges) {
        std::cout << "(" << edge[0] << ", " << edge[1] << ")    ("
                  << edge[2] << ", " << edge[3] << ")\n";
    }

    return {vertices, edges, labels};
}

void print_graph(const std::vector<std::vector<std::vector<int>>>& graph) {
    if (graph.size() != 3) {
        std::cout << "Error: Invalid graph structure\n";
        return;
    }

    std::cout << "Graph structure:\n";
    
    // Print vertices
    std::cout << "Vertices:\n";
    for (size_t i = 0; i < graph[0].size(); ++i) {
        if (graph[0][i].size() >= 2) {
            std::cout << "  " << i + 1 << ": (" << graph[0][i][0] << "," << graph[0][i][1] << ")\n";
        }
    }
    
    // Print edges
    std::cout << "Edges:\n";
    for (size_t i = 0; i < graph[1].size(); ++i) {
        const auto& edge = graph[1][i];
        if (edge.size() >= 4) {
            std::cout << "  " << i + 1 << ": (" << edge[0] << "," << edge[1] << ") -> ("
                      << edge[2] << "," << edge[3] << ")\n";
        } else {
            std::cout << "  " << i + 1 << ": Invalid edge format\n";
        }
    }
    
    // Print labels
    std::cout << "Labels:\n";
    for (size_t i = 0; i < graph[2].size(); ++i) {
        std::cout << "  " << i + 1 << ": [";
        for (size_t j = 0; j < graph[2][i].size(); ++j) {
            std::cout << graph[2][i][j];
            if (j < graph[2][i].size() - 1) std::cout << ",";
        }
        std::cout << "]\n";
    }
}

// Function to get vertices from web structure
std::vector<std::string> getVertices(const std::vector<std::vector<OneSector>>& web) {
    std::vector<std::string> vertices;
    for (size_t i = 0; i < web.size(); ++i) {
        for (size_t j = 0; j < web[i].size(); ++j) {
            // Format as "11", "21", etc.
            std::string vertex = std::to_string(i + 1) + std::to_string(j + 1);
            vertices.push_back(vertex);
        }
    }
    return vertices;
}

// Function to get edges from web structure
std::vector<std::pair<std::string, std::string>> getEdges(const std::vector<std::vector<OneSector>>& web) {
    std::vector<std::pair<std::string, std::string>> edges;
    
    for (size_t i = 0; i < web.size() - 1; ++i) {
        for (size_t j = 0; j < web[i].size(); ++j) {
            for (size_t k = 0; k < web[i + 1].size(); ++k) {
                if (isSubset(web[i + 1][k].lab, web[i][j].lab)) {
                    std::string from = std::to_string(i + 1) + std::to_string(j + 1);
                    std::string to = std::to_string(i + 2) + std::to_string(k + 1);
                    edges.emplace_back(from, to);
                }
            }
        }
    }

    return edges;
}

int main() {
    std::cout << "Processing user input example:" << std::endl;
    /* 
    std::vector<std::vector<int>> input_sectors = {
        {1,1,1,1,1,1,1,-5,0},
        {1,1,1,1,1,1,1,0,-5},
        {0,1,1,1,1,1,1,-5,0},
        {1,0,1,1,1,1,1,0,-5},
        {1,1,0,1,1,1,1,-5,0},
        {1,1,1,0,1,1,1,0,-5},
        {1,1,1,1,0,1,1,-5,0},
        {1,1,1,1,1,0,1,0,-5},
        {1,1,1,1,1,1,0,-5,0},
        {0,1,1,1,1,1,1,0,-5},
        {1,0,1,1,1,1,1,-5,0},
        {1,1,0,1,1,1,1,0,-5},
        {1,1,1,0,1,1,1,-5,0},
        {1,1,1,1,0,1,1,0,-5},
        {1,1,1,1,1,0,1,-5,0},
        {1,1,1,1,1,1,0,0,-5}
    };
     */
    std::vector<std::vector<int>> input_sectors = {
        {1, 1, 1, 1, -3, -1, -1, -1, -1},
        {1, -1, 1, -1, -3, -1, -1, -4, -1}
    };

    // Process input sectors and generate web structure
    std::vector<std::vector<OneSector>> web = generateWebStructure(input_sectors);
    
    // Get vertices and edges from web structure
    std::vector<std::string> vertices = getVertices(web);
    std::vector<std::pair<std::string, std::string>> edges = getEdges(web);
    
    // Print vertices and edges in the exact format
    std::cout << "Vertices:\n{";
    for (size_t i = 0; i < vertices.size(); ++i) {
        std::cout << "\"" << vertices[i] << "\"";
        if (i != vertices.size() - 1) std::cout << ", ";
    }
    std::cout << "}\n";
    
    std::cout << "Edges:\n{";
    for (size_t i = 0; i < edges.size(); ++i) {
        std::cout << "{\"" << edges[i].first << "\", \"" << edges[i].second << "\"}";
        if (i != edges.size() - 1) std::cout << ", ";
    }
    std::cout << "}\n";
    
    return 0;
}

// Modify generateWebStructure to return the web structure
std::vector<std::vector<OneSector>> generateWebStructure(const std::vector<std::vector<int>>& input_sectors) {
    std::vector<std::vector<OneSector>> web;
    
    // First, get the positive indices from the first sector
    std::vector<int> positive_indices;
    for (size_t i = 0; i < input_sectors[0].size(); ++i) {
        if (input_sectors[0][i] > 0) {
            positive_indices.push_back(i + 1);  // Using 1-based indexing
        }
    }
    
    // Generate layers
    for (size_t layer = 1; layer <= positive_indices.size(); ++layer) {
        std::vector<OneSector> layer_sectors;
        
        // Generate combinations of size (positive_indices.size() - layer + 1)
        std::vector<std::vector<int>> combinations;
        generateCombinations(positive_indices, positive_indices.size() - layer + 1, combinations);
        
        // Create sectors in this layer
        for (size_t i = 0; i < combinations.size(); ++i) {
            OneSector sector;
            sector.lab = combinations[i];
            sector.sectorMap = {};
            sector.targetInts = {};
            layer_sectors.push_back(sector);
        }
        
        web.push_back(layer_sectors);
    }
    
    // Print the web structure
    print_web(web);
    
    // Generate and print graph structure
    std::cout << "\nGenerating graph:" << std::endl;
    generateGraph(positive_indices);
    
    return web;
}

void generateCombinations(const std::vector<int>& elements, size_t r, 
                         std::vector<std::vector<int>>& result) {
    std::vector<int> combination;
    generateCombinationsHelper(elements, r, 0, combination, result);
}

void generateCombinationsHelper(const std::vector<int>& elements, size_t r,
                              size_t start, std::vector<int>& combination,
                              std::vector<std::vector<int>>& result) {
    if (combination.size() == r) {
        result.push_back(combination);
        return;
    }
    
    for (size_t i = start; i < elements.size(); ++i) {
        combination.push_back(elements[i]);
        generateCombinationsHelper(elements, r, i + 1, combination, result);
        combination.pop_back();
    }
}

void generateGraph(const std::vector<int>& positive_indices) {
    // Generate vertices
    std::cout << "size of web T: " << positive_indices.size() << std::endl;
    
    // Print vertices
    std::cout << "Vertices:" << std::endl;
    size_t vertex_count = 1;
    for (size_t layer = 1; layer <= positive_indices.size(); ++layer) {
        std::vector<std::vector<int>> combinations;
        generateCombinations(positive_indices, positive_indices.size() - layer + 1, combinations);
        
        for (size_t i = 0; i < combinations.size(); ++i) {
            std::cout << "(" << layer << ", " << (i + 1) << ")    ({";
            for (size_t j = 0; j < combinations[i].size(); ++j) {
                std::cout << combinations[i][j];
                if (j < combinations[i].size() - 1) std::cout << ",";
            }
            std::cout << "})" << std::endl;
            vertex_count++;
        }
    }
    
    // Generate and print edges
    std::cout << "Edges:" << std::endl;
    for (size_t layer = 1; layer < positive_indices.size(); ++layer) {
        std::vector<std::vector<int>> current_combinations;
        std::vector<std::vector<int>> next_combinations;
        generateCombinations(positive_indices, positive_indices.size() - layer + 1, current_combinations);
        generateCombinations(positive_indices, positive_indices.size() - layer, next_combinations);
        
        for (size_t i = 0; i < current_combinations.size(); ++i) {
            for (size_t j = 0; j < next_combinations.size(); ++j) {
                if (isSubset(next_combinations[j], current_combinations[i])) {
                    std::cout << "(" << layer << ", " << (i + 1) << ")    (" 
                             << (layer + 1) << ", " << (j + 1) << ")" << std::endl;
                }
            }
        }
    }
}

bool isSubset(const std::vector<int>& small, const std::vector<int>& big) {
    for (int val : small) {
        if (std::find(big.begin(), big.end(), val) == big.end()) {
            return false;
        }
    }
    return true;
}
