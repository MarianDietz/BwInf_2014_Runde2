#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <cstring>
#include <algorithm>

#include "Buschfeuer.h"

Point getOptimalWaterSpot(std::vector<Point>& candidates){
  std::queue<std::pair<PII,Point> > q;                               // ((distance | color) | Location)
  for(int i= 0; i < candidates.size(); ++i)
    q.push(std::pair<PII,Point>(PII(0,i),candidates[i]));       // insert all the candidates as start points for the BFS

  std::vector<std::vector<std::set<int> > > visited(Forest.width(),       // remember all nearest points first
    std::vector<std::set<int> >(Forest.height()));
  std::vector<std::vector<int> > shortDis(Forest.width(),            // shortest distant to any burning field
    std::vector<int>(Forest.height(),oo));

  //BFS to calculate shortest paths
  while(!q.empty()){
    std::pair<PII,Point> ac = q.front();
    Point acPoint = ac.second;
    int acDistance = ac.first.first;
    int acColor = ac.first.second;

    q.pop();
    if(visited[acPoint.x][acPoint.y].count(acColor))
      continue;
    visited[acPoint.x][acPoint.y].insert(acColor);
    shortDis[acPoint.x][acPoint.y] = std::min(acDistance,shortDis[acPoint.x][acPoint.y]);

    for(int i= 0; i < 4; ++i){
      int newx = acPoint.x + dir[i][0];
      int newy = acPoint.y + dir[i][1];                     // calculate new field's indexes
      
      if(newx < 0 || newy < 0 || newy >= Forest.height() || newx >= Forest.width())
	continue;                                           // new field is outside the woods
      if (Forest(newx, newy) != BURNABLE)
	continue;                                           // Field is not of interest
	
      if(visited[newx][newy].count(acColor) == 0)           // Don't compute things twice
	if(acDistance + 1 <= shortDis[newx][newy]){
	  shortDis[newx][newy] = acDistance + 1;
	  q.push(std::pair<PII,Point>(PII(acDistance + 1,acColor),Point(newx,newy)));
	}
    }
  }

  //determine the field to be watered
  std::vector<PII> waterval(candidates.size(),PII(0,0));
  std::vector<std::vector<int> > farthDist(candidates.size(), std::vector<int>(2*(Forest.width()+Forest.height()),0));
  
  //Count the number of fields that have an unique fire spot a.k.a. waterval
  //Reckon the farthest field
  for(int i= 0; i< Forest.width(); ++i)
    for(int j= 0; j < Forest.height(); ++j)
      if(visited[i][j].size() == 1){
	waterval[*visited[i][j].begin()].first++;
	farthDist[*visited[i][j].begin()][shortDis[i][j]]++;
      }
  for(int i = 0; i < waterval.size(); ++i)
    waterval[i].second = i;
  
  std::sort(waterval.begin(),waterval.end(),std::greater<PII>());
  
  return candidates[waterval[0].second];
}

std::vector<Point>& getInitialBurningFields() {
  static std::vector<Point> burnedFields;
  
  for(int i = 0; i < Forest.height(); ++i)
    for(int j= 0; j < Forest.width(); ++j)
      if(Forest(j, i) & BURNED){
	burnedFields.push_back(Point(j, i));
// 	printf("Initially burning: (%i|%i)\n",j, i);
      }
  return burnedFields;
}
  
void simulateFire(const std::vector<Point>& initiallyBurningFields) {
  std::vector<Point> burnedFields = initiallyBurningFields;
  if(printSolution != dontPrintSolution)
    printSolution_TERMINAL(stdout, false);
  if (OUT != 0)
    printSolution(OUT, false);
  
  int time = 0;
  while(!burnedFields.empty()) {                           // Simulate as long as there's still fire in the world
    std::vector<Point> newBurnedFields;                         // The burning fields at the next point of time
    
    //Calculate the new burning fields
    for(size_t i = 0; i < burnedFields.size(); ++i){
      int acx = burnedFields[i].x;
      int acy = burnedFields[i].y;
      
      if(Forest(acx, acy) & WATERED)
	continue;                                          // The field got watered and does not spread fire
      Forest(acx, acy) |= COAL;                            // Field burned down to coal...

      for(int j = 0; j < 4; ++j) {
	int newx = acx + dir[j][0];
	int newy = acy + dir[j][1];
	
	if(newx < 0 || newy < 0 || newy >= Forest.height() || newx >= Forest.width())
	  continue;                                        // new field is outside the woods
	if(Forest(newx, newy) == BURNABLE){
	  Forest(newx, newy) |= BURNED;                    // Field starts burning
	  newBurnedFields.push_back(Point(newx,newy));
	  
//  	  printf("  From now on burning: (%i|%i)\n",newx,newy);     // log the happenings
	}
      }
    }
    if(newBurnedFields.empty())                           // Nothing to water, all plants happy...
	break;
	
    burnedFields = newBurnedFields;
        
    Point toWater = getOptimalWaterSpot(newBurnedFields); // Determine the field to water
    Forest(toWater.x, toWater.y) |= (WATERED | (time << 4));              // ... and water it
    
    
    //Output / mirror the partial solution
    
    std::printf("---\nAt time %i: Water spot (%i|%i)\n",++time,toWater.x,toWater.y);
    if(printSolution != dontPrintSolution)
      printSolution_TERMINAL(stdout, false);
    
    if (OUT) {
      std::fprintf(OUT, "---\nAt time %i: Water spot (%i|%i)\n",time,toWater.x,toWater.y);
      printSolution(OUT, false);
    }

  }
//   printf("Fire died.\n");
  
  //Count the total number of burned or coaled
  int wcnt = 0, ccnt = 0;
  for(int i= 0; i < Forest.width(); ++i)
    for(int j= 0; j < Forest.height(); ++j)
      if(Forest(i, j) & WATERED)
	wcnt++;
      else if(Forest(i, j) & COAL)
	ccnt++;
	
  //Output / Mirror the solution
  std::printf("---\nAnd you'll find %i pieces of coal and %i pieces of watered coal\n",ccnt,wcnt);  
  if(printSolution != dontPrintSolution)
    printSolution_TERMINAL(stdout, true);
  if (OUT) {
    std::fprintf(OUT, "---\nAnd you'll find %i pieces of coal and %i pieces of watered coal\n",ccnt,wcnt);
    printSolution(OUT, true);
  }
}
  
int main(int argc, char** argv){
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
  simulateFire(getInitialBurningFields());
}