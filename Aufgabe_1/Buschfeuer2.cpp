#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <cstring>
#include <algorithm>
#include <map>

#include "Buschfeuer.h"

typedef std::tuple<int,int,Woods> sssType; //State-Space-Search: distance from source; #of skipped waterings, acForest
typedef std::pair<int,int> pii;

bool operator <(const sssType &a, const sssType &b){
  
  int aBurning = std::get<2>(a).cnt();
  int bBurning = std::get<2>(b).cnt();
  
  int aBurned = std::get<2>(a).cnt2();
  int bBurned = std::get<2>(b).cnt2();
  
  auto aSk = std::get<1>(a);
  auto bSk = std::get<1>(b);
  
  return (aBurned > bBurned || (aBurned == bBurned && (aBurning - aSk > bBurning - bSk)));
}

std::pair<Woods,std::vector<Point>> getNextState(const Woods& w){
     //Calculate the next state's fire paired with the positions
     //of the new fire spots
  Woods ret(w.width(),w.height());
  std::vector<Point> pnts;
  
  for(int i = 0; i < w.width(); ++i)
    for(int j = 0; j < w.height(); ++j){
      ret(i,j) = w(i,j);
      if(w(i,j) == BURNABLE){
	bool startsBurning = false;
	for(int k = 0; k < 4; ++k){
	  int x = i + dir[k][0];
	  int y = j + dir[k][1];
	  if(x < 0 || y < 0 || x >= w.width() || y >= w.height())
	    continue;
	  if(w(x,y) & BURNED && !(w(x,y) & WATERED))
	    startsBurning = true;
	}
	if(startsBurning){
	  ret(i,j) |= BURNED;
	  pnts.push_back(Point(i,j));
	}
      }
      else if(w(i,j) & BURNED && !(w(i,j) & WATERED))
	ret(i,j) |= COAL;
    }
  return std::pair<Woods,std::vector<Point>>(ret, pnts);
}

int main(int argc, char ** argv){
  if (argc > 1) {
    std::freopen(argv[1],"r",stdin);
    std::printf("Using %s as input.\n", argv[1]);
  }
  if (argc > 2){
    std::printf("Mirroring output to %s.\n", argv[2]);
    if (strstr(argv[2], ".tex2")) {
      std::printf("I reckon you want me to produce some graphicless TeX stuff...\n");
      printSolution = dontPrintSolution;
    }
    else if (strstr(argv[2], ".tex")) {
      std::printf("I reckon you want me to produce some TeX stuff...\n");
      printSolution = printSolution_TEX;
    }
    else if (strstr(argv[2], ".raw")) {
      std::printf("I reckon you want me to surpress graphics...\n");
      printSolution = dontPrintSolution;
    }
    else
      printSolution = printSolution_TERMINAL;
    OUT = std::fopen(argv[2],"w");
  }
  else{
    OUT = 0;
    printSolution = printSolution_TERMINAL;
  }

  parseInput(stdin);
  
  std::priority_queue<sssType> q; //The SSS-Queue
  q.push(sssType(1,0,Forest)); //Initial Node, let time start at 1
    
  while(!q.empty()) {
    sssType ac = q.top(); q.pop();
    
    auto acForest = std::get<2>(ac);
    int acDis = std::get<0>(ac);
    int acSkipped = std::get<1>(ac);
    
    auto next = getNextState(acForest); 
    
    
    if(acForest.cnt() <= acSkipped)
    { //Fire can be dead by this time
      //Reconstruct and output Solution
      
      Forest = acForest;
      
      std::set<int> remTimes; //check at which points in time a watering was required
                          //so that those fields that are still burning can get one of the
                          //remaining times to be watered
                          
      for(int i = 1; i<= acDis; ++i)
	remTimes.insert(i);
      
      
      for(int i = 0; i< Forest.width(); ++i)
        for(int j = 0; j < Forest.height(); ++j)
	  if((Forest(i,j) & BURNED) && ( Forest(i,j) & WATERED ) )
	    remTimes.erase(remTimes.find(Forest(i,j) >> 4));
      
      int ccnt = 0;  //No of coaled tiles

      for(int i = 0; i< Forest.width(); ++i)
        for(int j = 0; j < Forest.height(); ++j)
	  if((Forest(i,j) & BURNED) && !( Forest(i,j) & (WATERED | COAL )) ){
	    Forest(i,j) |= (WATERED | ((*remTimes.begin()) << 4));
	    remTimes.erase(remTimes.begin());
	  }
	  else if ((Forest(i,j) & COAL) && !( Forest(i,j) & WATERED) )
	    ccnt++;
	  
      std::printf("Water always optimally (to save water)...\nand you'll find %i pieces of coal and %i pieces of watered coal\n",ccnt,acDis-acSkipped -1);  

      if(printSolution != dontPrintSolution)
	printSolution_TERMINAL(stdout, true);
      if (OUT) {
	std::fprintf(OUT, "Water always optimally (to save water)...\nAnd you'll find %i pieces of coal and %i pieces of watered coal\n",ccnt,acDis- acSkipped-1);
	printSolution(OUT, true);
      }
      continue;
//       break;
    }
    
    q.push(sssType(acDis + 1,acSkipped + 1, next.first));
    for(auto i : next.second){
      next.first(i.x,i.y) |= (WATERED | (acDis << 4)); // Save time whe field got watered
      q.push(sssType(acDis+ 1,acSkipped, next.first));
      next.first(i.x,i.y) &= ~(WATERED | (acDis << 4));
    }
  }
  printf("Finished\n");
}