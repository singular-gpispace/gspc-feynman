void printWeb(const Web& web);
void printGraphData(const GraphData& graph);
void printDAG(const DAG& dag);
void printPetriNet(const PetriNet& net);

std::vector<std::vector<std::vector<int>>> pickHighestSector(const std::vector<std::vector<int>>& targetInt) {
    std::vector<std::vector<std::vector<int>>> sortedInt1; // List of sector labels
    std::vector<std::vector<std::vector<int>>> sortedInt2; // List of integral collections
    std::vector<std::vector<std::vector<int>>> intsWithSectors;
    int isSubList(const std::vector<int>& l1, const std::vector<int>& l2);
    std::vector<std::vector<int>> getSector(const std::vector<int>& targetInt);
    std::vector<std::vector<int>> lexSort(const std::vector<std::vector<std::vector<int>>>& intsWithSectors);
    std::vector<int> deleteElement(const std::vector<int>& vec, int index);
    std::vector<std::vector<int>> listCombinations(const std::vector<int>& L, int r);
    std::vector<std::vector<oneSector>> generateWebSectors(const std::vector<int>& seed);
    std::vector<std::vector<int>> getCombinations(const std::vector<int>& L, int r, const std::vector<int>& pre);