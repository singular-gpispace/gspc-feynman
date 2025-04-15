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
