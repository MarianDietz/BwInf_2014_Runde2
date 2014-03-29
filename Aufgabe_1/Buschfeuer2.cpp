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

pair<Woods,vector<Point>> getNextState(const Woods& w){
  Woods ret(w.width(),w.height());
  vector<Point> pnts;
  
  for(int i = 0; i < w.width; ++i)
    for(int j = 0; j < w.height; ++j){
      ret(i,j) = w(i,j);
      if(w(i,j) == BURNABLE){
	bool startsBurning = false;
	for(int k = 0; k < 4; ++k){
	  int x = i + dir[k][0];
	  int y = j + dir[k][1];
	  if(x < 0 || y < 0 || x >= w.width() || y >= w.height())
	    continue;
	  if(w(x,y) & BURNED)
	    startsBurning = true;
	}
	if(startsBurning){
	  ret(i,j) |= BURNED;
	  pnts.push_back(Point(i,j));
	}
      }
    }
  return pair<Woods,vector<Point>>(ret, pnts);
}

int main(){
  if (argc > 1) {
    std::freopen(argv[1],"r",stdin);
    std::printf("Using %s as input.\n", argv[1]);
  }
  if (argc > 2){
    printf("Mirroring output to %s.\n", argv[2]);
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
  
  std::queue<sssType> q;
  q.push(sssType(0,0,Forest));
  set<Woods> visited;
  
  while(!q.empty()){
    sssType ac = q.front(); q.pop();
    if(visited.count(get<2>(ac))) //case already calculated
      continue;
    visited.insert(get<2>(ac));
    
    auto next = getNextState(get<2>(ac));
    if(next.second.size() <= get<1>(ac) + 1){ //Fire can be dead by this state
      //Reconstruct Solution Here
      Forest = get<2>(ac);
      break;
    }
    
    q.push(sssType(get<0>(ac) + 1,get<1>(ac) + 1, next.first));
    for(auto i : next.second){
      next.first(i.first,i.second) |= WATERED;
      q.push(sssType(get<0>(ac) + 1,get<1>(ac), next.first));
      next.first(i.first,i.second) &= ~WATERED;
    }
  }
  
}