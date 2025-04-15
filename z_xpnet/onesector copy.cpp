

proc web_g(list targetInt)
{


  list L = pickHighestSector(targetInt[1]);

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
    return(list(reducedIBPs, MIs, OutputtailInts));
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
  
  return(list(reducedIBPs, MIs, OutputtailInts));
}


example{"EXAMPLE:"; echo=2;
//Setting the graph details and Baikov computations

 graph  G = makeGraph(list(1,2,3,4,5,6),list(list(6,1),list(4,6),list(1,2),list(3,5),list(4,3),list(2,5),list(5,6),list(1),list(2),list(3),list(4)));
  list userInput;
   //userInput[1]=list(list(1,1,1,-1,-1,-1,-1,0,0),list(1,-1,1,-1,-1,-1,-1,0,0));
   list tail_00 = list(list(1, 1, 1, -1, -3, -1, -1, -1, -1), list(1, -1, 1, -1, -3, -1, -1, -4, -1));
   userInput[1]=tail_00;
  labeledgraph G1=computeBaikovMatrix(G);
  ring RB=G1.baikovover;


//sector at(1,1), label:{1,2,3} computations
  list web=web_g(userInput);
  list label_11=getlabels(web,1,1);
  list SectOutput_11=OneSectorComputation(G1,userInput,label_11);
  list tail_11=SectOutput_11[3];

//For sector at(2,1), label:{1,2} computations
  list input_21;
  input_21[1]=tail_00;
  input_21[2]=tail_11;
  list label_21=getlabels(web,2,1);
  list SectOutput_21=OneSectorComputation(G1,input_21,label_21);
  list tail_21=SectOutput_21[3];

//For sector at(2,2), label:{2,3} computations
  list input_22;
  input_22[1]=tail_00;
  input_22[2]=tail_11;
  list label_22=getlabels(web,2,2);
  list SectOutput_22=OneSectorComputation(G1,input_22,label_22);
  list tail_22=SectOutput_22[3];
  
//For sector at(2,3), label:{1,3} computations
  list input_23;
  input_23[1]=tail_00;
  input_23[2]=tail_11;
  list label_23=getlabels(web,2,3);
  list SectOutput_23=OneSectorComputation(G1,input_23,label_23);
  list tail_23=SectOutput_23[3];

//For sector at(3,1) , label :{1} computations
  list input_31; //Note that sector at (3,1) has two parents ({1,2} and {1,3}) and one grandparent ({1,2,3})
  input_31[1]=tail_00;
  input_31[2]=tail_21;
  input_31[3]=tail_23;
  input_31[4]=tail_11;
  list label_31=getlabels(web,3,1);
  list SectOutput_31=OneSectorComputation(G1,input_31,label_31);
  list tail_31=SectOutput_31[3];

//For sector at(3,2), label:{2} computations
  list input_32;
  input_32[1]=tail_00;
  input_32[2]=tail_21;
  input_32[3]=tail_22;
  input_32[4]=tail_11;
  list label_32=getlabels(web,3,2);
  list SectOutput_32=OneSectorComputation(G1,input_32,label_32);
  list tail_32=SectOutput_32[3];

//For sector at(3,3), label:{3} computations
  list input_33;
  input_33[1]=tail_00;
  input_33[2]=tail_22;
  input_33[3]=tail_23;
  input_33[4]=tail_11;
  list label_33=getlabels(web,3,3);
  list SectOutput_33=OneSectorComputation(G1,input_33,label_33);
  list tail_33=SectOutput_33[3];
}
 std::vector<std::string> vertices = {"11", "21", "22", "23", "31", "32", "33"};
    std::vector<std::pair<std::string, std::string>> edges = {
        {"11", "21"}, {"11", "22"}, {"11", "23"},
        {"21", "31"}, {"21", "32"}, {"22", "32"}, {"22", "33"}, {"23", "31"}, {"23", "33"} 
    };