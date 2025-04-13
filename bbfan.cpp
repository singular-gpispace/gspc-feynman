//************************************************************************************************************************** */
// Helper Functions
//************************************************************************************************************************** */

/**
 * Checks if small is a subset of big
 * @param small: Potential subset list
 * @param big: Containing list
 * @return: 1 if subset, 0 otherwise
 */
proc subset(list small, list big) {
  for (int i = 1; i <= size(small); i++) {
      if (!contains(big, small[i])) {
          return (0);
      }
  }
  return (1);
}

/**
* Checks if list L contains element x
* @param L: List to search
* @param x: Element to find
* @return: 1 if found, 0 otherwise
*/
proc contains(list L, int x) {
  for (int i = 1; i <= size(L); i++) {
      if (L[i] == x) {
          return (1);
      }
  }
  return (0);
}

//************************************************************************************************************************** */
// Graph Generation Function
//************************************************************************************************************************** */

/**
* Generates graph structure from web T, combining vertices and labels
* @param T: Web structure representing sector hierarchy
* @return: List containing verticesWithLabels and edges
*/
proc getGraph(list T) {
  list verticesWithLabels = list();
  list edges = list();
  int v_idx = 1;
  int e_idx = 1;
  
  // Log web size
  print("Size of web T: " + string(size(T)));
  
  // Generate vertices with labels
  for (int i = 1; i <= size(T); i++) {
      print("Size of layer " + string(i) + ": " + string(size(T[i])));
      for (int j = 1; j <= size(T[i]); j++) {
          // Store vertex coordinates and label together
          verticesWithLabels[v_idx] = list(list(i, j), T[i][j].lab);
          v_idx++;
      }
  }
  
  // Print vertices with labels
  print("Vertices with Labels:");
  for (int i = 1; i <= size(verticesWithLabels); i++) {
      list vertex = verticesWithLabels[i][1];
      list label = verticesWithLabels[i][2];
      print("(" + string(vertex[1]) + ", " + string(vertex[2]) + ")" + 
            "    Label: " + string(label));
  }
  
  // Generate edges based on subset relationships
  for (int i = 1; i < size(T); i++) {
      for (int j = 1; j <= size(T[i]); j++) {
          for (int k = 1; k <= size(T[i+1]); k++) {
              if (subset(T[i+1][k].lab, T[i][j].lab)) {
                  edges[e_idx] = list(list(i,j), list(i+1,k));
                  e_idx++;
              }
          }
      }
  }
  
  // Print edges
  print("Edges:");
  for (int i = 1; i <= size(edges); i++) {
      print("(" + string(edges[i][1][1]) + ", " + string(edges[i][1][2]) + ")" + 
            " -> (" + string(edges[i][2][1]) + ", " + string(edges[i][2][2]) + ")");
  }
  
  return (list(verticesWithLabels, edges));
}

//************************************************************************************************************************** */
// Example Usage
//************************************************************************************************************************** */

example {
  // Mock web structure for testing (adjust based on actual T structure)
  list T = list(
      list(list(lab=list(1,2,3))), // Layer 1: sector {1,2,3}
      list(list(lab=list(1,2)), list(lab=list(1,3)), list(lab=list(2,3))), // Layer 2
      list(list(lab=list(1)), list(lab=list(2)), list(lab=list(3))) // Layer 3
  );
  
  // Generate graph
  list graphData = getGraph(T);
  
  // Access results
  list verticesWithLabels = graphData[1];
  list edges = graphData[2];
  
  print("Result Summary:");
  print("Vertices with Labels: " + string(verticesWithLabels));
  print("Edges: " + string(edges));
}

proc OneSectorComputation(labeledgraph G1,list tailInts,list sector_lab )
"USAGE:  
ASSUME:  
RETURN:  
KEYWORDS: Feynman graph,IBPs
"
{ 
  list targetInt;
  setIBP reducedIBPs; //a place to store the independent IBPs coming out from current sector
  list MIs;  //a place to store the Master integrals coming out from current sector
  list OutputtailInts;  //a place to store the tail integrals coming out from current sector
  
//1.Extract target integrals for current sector (which are tail integrals coming from parent 
//sectors)

  for(int i=1;i<=size(tailInts);i++)
  {
    for(int j=1;j<=size(tailInts[i]);j++)
    {
            if(string(getSector(tailInts[i][j])[2])==string(sector_lab)){
        targetInt[size(targetInt)+1]=tailInts[i][j];
      }
    }

  }

  if(size(targetInt)==0){
    print("No input integrals came from the parents")
    return(reducedIBPs,MIs,OutputtailInts);
  }
  

//2. Computations related to sector

  setIBP totalIBP=computeManyIBP(G1,targetInt);
  list L=getRedIBPs(totalIBP,7853);    //L[1]=,set of independent IBPs, L[2]=master and tail integrals
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

  
  return(list(reducedIBPs,MIs,OutputtailInts));

}
example{"EXAMPLE:"; echo=2;
//Setting the graph details and Baikov computations

  graph  G = makeGraph(list(1,2,3,4,5,6),list(list(6,1),list(4,6),list(1,2),list(3,5),list(4,3),list(2,5),list(5,6),list(1),list(2),list(3),list(4)));
  list userInput;
   userInput[1]=list(list(1,1,1,-1,-1,-1,-1,0,0),list(1,-1,1,-1,-1,-1,-1,0,0));
  labeledgraph G1=computeBaikovMatrix(G);
  ring RB=G1.baikovover;
 
//sector at(1,1), label:{1,2,3} computations
  list SectOutput123=OneSectorComputation(G1,userInput,list(1,2,3));
  list tail123=SectOutput123[3];

//For sector at(2,1), label:{1,2} computations
  list input12;
  input12[1]=userInput[1];
  input12[2]=tail123;
  list SectOutput12=OneSectorComputation(G1,input12,list(1,2));
  list tail12=SectOutput12[3];

//For sector at(2,2), label:{2,3} computations
  list input23;
  input23[1]=userInput[1];
  input23[2]=tail123;
  list SectOutput23=OneSectorComputation(G1,input23,list(2,3));
  list tail23=SectOutput23[3];
  
//For sector at(2,3), label:{1,3} computations
  list input13;
  input13[1]=userInput[1];
  input13[2]=tail123;
  list SectOutput13=OneSectorComputation(G1,input13,list(1,3));
  list tail13=SectOutput13[3];

//For sector at(3,1) , label :{1} computations
  list input31; //Note that sector at (3,1) has two parents ({1,2} and {1,3}) and one grandparent ({1,2,3})
  input31[1]=userInput[1];
  input31[2]=tail12;
  input31[3]=tail13;
  input31[4]=tail123;
  list SectOutput1=OneSectorComputation(G1,input31,list(1));
  list tail1=SectOutput1[3];

//likewise can go through all the sectors
}
