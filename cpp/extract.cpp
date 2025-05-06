#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

struct OneSector {
    std::vector<int> lab;
    std::vector<std::vector<int>> sectorMap;
    std::vector<int> targetInts;
};

// Forward declarations
std::vector<OneSector> web_g(const std::vector<std::vector<std::vector<int>>>& targetInt);
std::vector<std::vector<int>> pickHighestSector(const std::vector<std::vector<int>>& targetInt);
std::vector<std::vector<int>> getSector(const std::vector<int>& l);
int isSubList(const std::vector<int>& l1, const std::vector<int>& l2);
int comp_lex(const std::vector<int>& l1, const std::vector<int>& l2);
std::vector<std::vector<std::vector<int>>> lexSort(const std::vector<std::vector<std::vector<int>>>& L);
std::vector<OneSector> generateWebSectors(const std::vector<int>& seed);
std::vector<std::vector<int>> listCombinations(const std::vector<int>& L, int r);
std::vector<std::vector<int>> getCombinations(const std::vector<int>& L, int r, const std::vector<int>& pre);
std::vector<int> getlabels(const std::vector<OneSector>& web, int i, int j);

std::vector<OneSector> web_g(const std::vector<std::vector<std::vector<int>>>& targetInt) {
    std::vector<std::vector<int>> L = pickHighestSector(targetInt[0]);
    std::vector<std::vector<int>> sectorInfo = getSector(L[0]);
    std::vector<int> indices = sectorInfo[1];
    return generateWebSectors(indices);
}

std::vector<std::vector<int>> pickHighestSector(const std::vector<std::vector<int>>& targetInt) {
    std::vector<std::vector<std::vector<int>>> intsWithSectors;
    for (size_t i = 0; i < targetInt.size(); ++i) {
        std::vector<std::vector<int>> L = getSector(targetInt[i]);
        std::vector<int> sector = L[1];
        std::vector<std::vector<int>> entry;
        entry.push_back(L[0]);
        entry.push_back(targetInt[i]);
        
        std::vector<std::vector<int>> temp;
        temp.push_back(L[0]);
        temp.push_back(targetInt[i]);
        intsWithSectors.push_back(temp);
    }

    std::vector<std::vector<std::vector<int>>> L = lexSort(intsWithSectors);
    std::vector<std::vector<int>> sortedInt1;
    std::vector<std::vector<int>> sortedInt2;

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
    return sortedInt2;
}

std::vector<std::vector<int>> getSector(const std::vector<int>& l) {
    std::vector<int> s(l.size(), 0);
    std::vector<int> n;
    for (size_t i = 0; i < l.size(); ++i) {
        if (l[i] > 0) {
            s[i] = 1;
            n.push_back(i); 
        }
        else
        {
            s[i]=0;
        }
    }
    return {s, n};
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

std::vector<OneSector> generateWebSectors(const std::vector<int>& seed) {
    std::vector<std::vector<int>> L = getSector(seed);
    std::vector<int> sector = L[0];
    int nSectors = 1 << L[1].size();
    std::vector<OneSector> sectorWeb;
    
    // Create the nested structure as in Singular
    for (size_t i = 0; i < L[1].size(); ++i) {
        // Get combinations of size L[1].size() - i
        std::vector<std::vector<int>> combinations = listCombinations(L[1], L[1].size() - i);
        
        // Create a list of sectors for this level
        for (const auto& comb : combinations) {
            OneSector newSector;
            newSector.lab = comb;
            newSector.targetInts = {};
            newSector.sectorMap = {};
            sectorWeb.push_back(newSector);
        }
    }
    
    return sectorWeb;
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

std::vector<int> getlabels(const std::vector<OneSector>& web, int i, int j) {
    return web[i].lab;
}

void print_web(const std::vector<OneSector>& web) {
    std::cout << " web is :\n";
    if (web.empty()) {
        std::cout << "  (empty list)\n";
        return;
    }

    // Group sectors by layer based on the size of their lab
    std::map<size_t, std::vector<OneSector>> layerMap;
    
    for (const auto& sector : web) {
        size_t layerSize = sector.lab.size();
        layerMap[layerSize].push_back(sector);
    }
    
    // Print the layers in reverse order (largest to smallest)
    size_t layerNum = 1;
    for (auto it = layerMap.rbegin(); it != layerMap.rend(); ++it) {
        const auto& sectors = it->second;
        
        std::cout << "  Layer " << layerNum << ":\n";
        for (size_t i = 0; i < sectors.size(); ++i) {
            const auto& sector = sectors[i];
            std::cout << "    Sector " << (i + 1) << ":\n";
            
            // Print lab
            std::cout << "      lab = ";
            for (size_t k = 0; k < sector.lab.size(); ++k) {
                std::cout << sector.lab[k];
                if (k < sector.lab.size() - 1) std::cout << ",";
            }
            std::cout << "\n";
            
            // Print sectorMap
            std::cout << "      sectorMap = \n";
            
            // Print targetInts
            std::cout << "      targetInts:\n";
        }
        layerNum++;
    }
}

int main() {
    
    std::vector<std::vector<std::vector<int>>> userInput = {
        {
            {1, 1, 1, -1, -3, -1, -1, -1, -1},
            {1, -1, 1, -1, -3, -1, -1, -4, -1}
        }
    };

    std::cout << "Starting sector at(1,1)\n";
    std::vector<OneSector> web = web_g(userInput);
    std::cout << "Web structure:\n";
    print_web(web);

    return 0;
}
