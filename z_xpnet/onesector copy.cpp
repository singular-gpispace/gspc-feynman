proc web_g(list targetInt)
{

  print("targetInt: " + string(targetInt[1]));
  list L = pickHighestSector(targetInt[1]);
  // print L
  print("L=pickHighestSector(targetInt[1]): " + string(L));
  print("L[1][1]: " + string(L[1][1]));

  list web = generateWebSectors(L[1][1]);

  return(web);
}


proc getlabels(list web, int i, int j)
{
  list labs = web[i][j].lab;
  return(labs);  
}



proc OneSectorComputation(labeledgraph G1, list tailInts, list sector_lab)
"USAGE:  OneSectorComputation(G1, tailInts, sector_lab); G1 labeledgraph, tailInts list of lists, sector_lab list
ASSUME:  G1 is a labeled graph, tailInts is a list of lists containing integrals, sector_lab is a list of sector labels
RETURN:  list(reducedIBPs, MIs, OutputtailInts) where reducedIBPs is a setIBP, MIs and OutputtailInts are lists
KEYWORDS: Feynman graph, IBPs"
{ 
  list targetInt;
  setIBP reducedIBPs;
  reducedIBPs.over = G1.baikovover;
  reducedIBPs.seed = sector_lab;
  reducedIBPs.IBP = list();
  list MIs;
  list OutputtailInts;
  
  // Extract target integrals for current sector
  for(int i = 1; i <= size(tailInts); i++) {
    
    for(int j = 1; j <= size(tailInts[i]); j++) {
      list current_sector = getSector(tailInts[i][j])[2];
     
      if(string(current_sector) == string(sector_lab)) {
        targetInt[size(targetInt) + 1] = tailInts[i][j];
      }
    }
  }

  if(size(targetInt) == 0) {
    print("No input integrals found for sector " + string(sector_lab));
    list result;
    result[1] = reducedIBPs;
    result[2] = MIs;
    result[3] = OutputtailInts;
    return(result);
  }
  
//2. Computations related to sector
  setIBP totalIBP=computeManyIBP(G1,targetInt);
  list L=getRedIBPs(totalIBP,7853);    //L[1]=set of independent IBPs, L[2]=master and tail integrals

  reducedIBPs.seed=sector_lab;
  reducedIBPs.over=G1.baikovover;
  reducedIBPs.IBP=L[1];
  for(int i=1;i<=size(L[2]);i++){
    if(string(getSector(L[2][i])[2])==string(sector_lab)){
      MIs[size(MIs)+1]=L[2][i];
    }
    else{
      OutputtailInts[size(OutputtailInts)+1]=L[2][i];
    }
  }
  
  list result;
  result[1] = reducedIBPs;
  result[2] = MIs;
  result[3] = OutputtailInts;
  return(result);
}

/****************************************************************************************************** */




// Create graph
graph G = makeGraph(
  list(1,2,3,4,5,6),
  list(
    list(6,1), list(4,6), list(1,2), list(3,5),
    list(4,3), list(2,5), list(5,6), list(1),
    list(2), list(3), list(4)
  )
);

// Initial input
list tail_00 = list(
  list(1, 1, 1, -1, -3, -1, -1, -1, -1),
  list(1, -1, 1, -1, -3, -1, -1, -4, -1)
);
list userInput; userInput[1] = tail_00;

labeledgraph G1 = computeBaikovMatrix(G);
ring RB = G1.baikovover;

print("starting sector at(1,1)");
list web = web_g(userInput);
list label_11 = getlabels(web, 1, 1);
rtimer = 0;
system("--ticks-per-sec", 1000); // ms timer
int t = rtimer;
list SectOutput_11 = OneSectorComputation(G1, userInput, label_11);

rtimer - t;
print("timer for sector computations in (1,1) ms: " + string(rtimer));

list tail_11 = SectOutput_11[3];

// === Dependency map (tailMap as pseudo-map) ===
list tailMap;
tailMap[1] = list("00", tail_00);
tailMap[2] = list("11", tail_11);

// === Define computation graph ===
// To skip a sector, just comment it out here
list sectorGraph;
sectorGraph[1] = list("21", list("00", "11"));
sectorGraph[2] = list("22", list("00", "11"));
sectorGraph[3] = list("23", list("00", "11"));
sectorGraph[4] = list("31", list("00", "21", "23", "11"));
sectorGraph[5] = list("32", list("00", "21", "22", "11"));
sectorGraph[6] = list("33", list("00", "22", "23", "11"));

// === Coordinates for sectors ===
list coords;
coords[1] = list("21", list(2, 1));
coords[2] = list("22", list(2, 2));
coords[3] = list("23", list(2, 3));
coords[4] = list("31", list(3, 1));
coords[5] = list("32", list(3, 2));
coords[6] = list("33", list(3, 3));

// === Filter valid sectors ===
list validSectors;
int idx = 1;
for (int i = 1; i <= size(sectorGraph); i++) {
  if (typeof(sectorGraph[i]) != "none") {
    validSectors[idx] = sectorGraph[i];
    idx++;
  }
}

// === Main computation loop ===
for (int i = 1; i <= size(validSectors); i++) {
  list sectorInfo = validSectors[i];
  string name = sectorInfo[1];
  list deps = sectorInfo[2];
  
  // Find sector coordinates
  int coord_i = -1;
  int coord_j = -1;
  for (int j = 1; j <= size(coords); j++) {
    if (coords[j][1] == name) {
      coord_i = coords[j][2][1];
      coord_j = coords[j][2][2];
      break;
    }
  }

  if (coord_i == -1) {
    print("Skipping sector " + name + " (missing coords)");
    continue;
  }

  print("starting sector at(" + string(coord_i) + "," + string(coord_j) + ")");

  // Collect dependency tails
  list input;
  for (int j = 1; j <= size(deps); j++) {
    string depName = deps[j];
    for (int k = 1; k <= size(tailMap); k++) {
      if (tailMap[k][1] == depName) {
        input[j] = tailMap[k][2];
        break;
      }
    }
  }

  list lbl = getlabels(web, coord_i, coord_j);
  rtimer = 0;
system("--ticks-per-sec", 1000); // ms timer
int t = rtimer;
  list result = OneSectorComputation(G1, input, lbl);
rtimer - t;
print("timer for sector computations in (" + string(coord_i) + "," + string(coord_j) + ") ms: " + string(rtimer));


  // Save tail for reuse
  int new_idx = size(tailMap) + 1;
  tailMap[new_idx] = list(name, result[3]);
}

