#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <cstring>
#include <algorithm>
using namespace std;

typedef pair<int,int> PII;

#define FIELDSTATE 	char
#define EMPTY		0
#define BURNABLE	1
#define BURNED		2
#define WATERED		4
#define COAL		8

const int oo = (1 << 29);                                  // The infinity

class Woods{
private:
  int Width, Height;
  vector<vector<FIELDSTATE> > Fields;
  
public:
  Woods(int width, int height) : Width(width), Height(height) {
    Fields.assign(height, vector<FIELDSTATE>(width, 0));
  }
  
  int width() const { return Width; }
  int height() const { return Height; }
  
  FIELDSTATE& operator() (int x, int y) {
    if (x < 0 || y < 0 || x >= width() || y >= height())
      printf("OUT OF BOUNDS 1");
    return Fields[y][x];
  }
  FIELDSTATE operator() (int x,  int y) const {
    if (x < 0 || y < 0 || x >= width() || y >= height())
      printf("OUT OF BOUNDS 2");
    return Fields[y][x];
  }
} Forest(0, 0);

struct Point {
public:
  int x, y; 
  Point(int _x,int _y) : x(_x), y(_y) { }
  };
int dir[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

vector<Point> Solution;
FILE* OUT;                                                 // The file to mirror the output to
void (*printSolution)(FILE*, bool);

Point getOptimalWaterSpot(vector<Point>& candidates){
  queue<pair<PII,Point> > q;                               // ((distance | color) | Location)
  for(int i= 0; i < candidates.size(); ++i)
    q.push(pair<PII,Point>(PII(0,i),candidates[i]));       // insert all the candidates as start points for the BFS

  vector<vector<set<int> > > visited(Forest.width(),       // remember all nearest points first
    vector<set<int> >(Forest.height()));
  vector<vector<int> > shortDis(Forest.width(),            // shortest distant to any burning field
    vector<int>(Forest.height(),oo));

  //BFS to calculate shortest paths
  while(!q.empty()){
    pair<PII,Point> ac = q.front();
    Point acPoint = ac.second;
    int acDistance = ac.first.first;
    int acColor = ac.first.second;

    q.pop();
    if(visited[acPoint.x][acPoint.y].count(acColor))
      continue;
    visited[acPoint.x][acPoint.y].insert(acColor);
    shortDis[acPoint.x][acPoint.y] = min(acDistance,shortDis[acPoint.x][acPoint.y]);

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
	  q.push(pair<PII,Point>(PII(acDistance + 1,acColor),Point(newx,newy)));
	}
    }
  }

  //determine the field to be watered
  vector<PII> waterval(candidates.size(),PII(0,0));
  vector<vector<int> > farthDist(candidates.size(), vector<int>(2*(Forest.width()+Forest.height()),0));
  
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
	
  //determine the field of the candidates which has the highest waterval
  
  sort(waterval.begin(),waterval.end(),greater<PII>());
  
  for(int i = 0; i < waterval.size(); ++i)
    for(int j = 1; j < candidates.size(); ++j)
      if(farthDist[waterval[i].second][j] > 1)
	return candidates[waterval[i].second];

  return candidates[waterval[0].second];
}

//BEGIN OF INPUT
void parseInput(FILE* f) {
  int acFieldWidth, acFieldHeight;
  fscanf(f, "%i %i\n",&acFieldWidth, &acFieldHeight);
  
  Forest = Woods(acFieldWidth, acFieldHeight);

  for(int i = 0; i < acFieldHeight; ++i){
    for(int j= 0; j < acFieldWidth; ++j){
      char c;
      fscanf(f, "%c",&c);
      c -= '0';
      Forest(j, i) = (FIELDSTATE) c;
    }
    if(i < acFieldHeight-1)
      fscanf(f, "\n");
  }
}
//END OF INPUT
//BEGIN OF OUTPUT
void printSolution_TEX(FILE* f, bool finalOut) {	
  fprintf(f, "\\\\\n");
  
  fprintf(f, "\\begin{tikzpicture}\n");
  fprintf(f, "\\tikzset{square matrix/.style={\n");
  fprintf(f, "matrix of nodes,\n");
  fprintf(f, "column sep=-\\pgflinewidth, row sep=-\\pgflinewidth,\n");
  fprintf(f, "nodes={draw,\n");
  fprintf(f, "minimum height=#1,\n");
  fprintf(f, "anchor=center,\n");
  fprintf(f, "text width=#1,\n");
  fprintf(f, "align=center,\n");
  fprintf(f, "inner sep=0pt\n");
  fprintf(f, "},\n");
  fprintf(f, "},\n");
  fprintf(f, "square matrix/.default=1.2cm\n");
  fprintf(f, "}\n");
  
  fprintf(f, "\\matrix[square matrix=1.4em] {\n");
  for(int j= 0; j < Forest.height(); ++j) {
    for(int i= 0; i < Forest.width(); ++i) {
      if(i)
	fprintf(f," &");
      
        FIELDSTATE acField = Forest(i, j);
	if(acField == EMPTY)
	  fprintf(f, "|[fill=white]|");
	else if(acField & WATERED)
	  fprintf(f, "|[fill=cyan]|");
	else if(acField & BURNABLE)
	  fprintf(f, "|[fill=green]|");
	  
	if(acField & COAL)
	  fprintf(f, "\\color[rgb]{0,0,0}");
	else if(acField & BURNED)
	  fprintf(f, "\\color[rgb]{1,0,0}");
	else if(acField == EMPTY)
	  fprintf(f, "\\color[gray]{0.5}");
	else if(acField & BURNABLE)
	  fprintf(f, "\\color[gray]{0.75}");
	  
	if(acField & WATERED){
	  for (int t = 0; t < Solution.size(); ++t)
	    if (Solution[t].x == i && Solution[t].y == j) {
	      fprintf(f, "\\textbf{%02d}",t+1);
	      break;
	    }	  
	}
	else if(acField & COAL)
	  fprintf(f, "\\textbf{CO}");
	else if(acField & BURNED)
	  fprintf(f, "\\textbf{BU}");
	else if(acField == EMPTY)
	  fprintf(f, "WA");
	else if(acField & BURNABLE)
	  fprintf(f, " FO");
	else
	  fprintf(f, "\\phantom{AA}");
      fprintf(f, "%%\n");
    }
   
    fprintf(f, "\\\\\n");
  }
  
  fprintf(f, "};\n\\end{tikzpicture}\\\\\n");
  
  if(finalOut){
    fprintf(f, "\\\\\nExplanation:");
    fprintf(f, "\\\\\n\\colorbox{white}{\\color[gray]{0.5}WA}  ---  EMPTY");
    fprintf(f, "\\\\\n\\colorbox{green}{\\color[gray]{0.5}FO}  ---  BURNABLE");
    fprintf(f, "\\\\\n\\colorbox{white}{\\color[rgb]{1,0,0}\\textbf{BU}}  ---  BURNED");
    fprintf(f, "\\\\\n\\colorbox{white}{\\color[rgb]{0,0,0}\\textbf{CO}}  ---  COAL (doubly burned)");
    fprintf(f, "\\\\\n\\colorbox{cyan}{\\#\\#}  ---  WATERED at time \\#\\#");
    fprintf(f, "\\\\\nFields can have more than 1 state.");
  }
}

void printSolution_TERMINAL(FILE* f, bool finalOut) {	
  fprintf(f, "\n");
  //The ASCII-magic starts here:
  for(int j= 0; j < Forest.height(); ++j) {
    for(int i= 0; i < Forest.width(); ++i) {
      FIELDSTATE acField = Forest(i, j);
      int waterval = 0;
      
      fprintf(f, "\x1b[s  ");
      if (acField == EMPTY)
	fprintf(f, "\x1b[u\x1b[37;47mWA");
      if (acField & BURNABLE)
	fprintf(f, "\x1b[u\x1b[32;42mFO");
      if (acField & BURNED)
	fprintf(f, "\x1b[u\x1b[1;5;31m/\\");
      if (acField & COAL)
	fprintf(f, "\x1b[u\x1b[1;4;5;30m/\\");
      if (acField & WATERED)
	for (int t = 0; t < Solution.size(); ++t)
	  if (Solution[t].x == i && Solution[t].y == j) {
	    fprintf(f, "\x1b[u\x1b[46m%02d", t+1);
	    break;
	  }
      fprintf(f, "\x1b[0;39;49m");
    }
   
    fprintf(f, "\n");
  }
  
  if (finalOut) {// An Explanation shall be printed
    fprintf(f, "\nExplanation:");
    fprintf(f, "\n\x1b[37;47mWA\x1b[39;49m  ---  EMPTY");
    fprintf(f, "\n\x1b[32;42mFO\x1b[39;49m  ---  BURNABLE");
    fprintf(f, "\n\x1b[1;5;31m/\\\x1b[0;39m  ---  BURNED");
    fprintf(f, "\n\x1b[1;4;5;30m/\\\x1b[0;39m  ---  COAL (doubly burned)");
    fprintf(f, "\n\x1b[46m##\x1b[0;39m  ---  WATERED at time ##");
    fprintf(f, "\nFields can have more than 1 state.");
  }
  fprintf(f, "\n");
}

void dontPrintSolution(FILE* f,bool finalOut) { return; }
//END OF OUTPUT

vector<Point>& getInitialBurningFields() {
  static vector<Point> burnedFields;
  
  for(int i = 0; i < Forest.height(); ++i)
    for(int j= 0; j < Forest.width(); ++j)
      if(Forest(j, i) & BURNED){
	burnedFields.push_back(Point(j, i));
// 	printf("Initially burning: (%i|%i)\n",j, i);
      }
  return burnedFields;
}
  
void simulateFire(const vector<Point>& initiallyBurningFields) {
  vector<Point> burnedFields = initiallyBurningFields;
  if(printSolution != dontPrintSolution)
    printSolution_TERMINAL(stdout, false);
  if (OUT != 0)
    printSolution(OUT, false);
  
  int time = 0;
  while(!burnedFields.empty()) {                           // Simulate as long as there's still fire in the world
    vector<Point> newBurnedFields;                         // The burning fields at the next point of time
    
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
    Forest(toWater.x, toWater.y) |= WATERED;              // ... and water it
    Solution.push_back(toWater);
    
    
    //Output / mirror the partial solution
    
    printf("---\nAt time %i: Water spot (%i|%i)\n",++time,toWater.x,toWater.y);
    if(printSolution != dontPrintSolution)
      printSolution_TERMINAL(stdout, false);
    
    if (OUT) {
      fprintf(OUT, "---\nAt time %i: Water spot (%i|%i)\n",time,toWater.x,toWater.y);
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
  printf("---\nAnd you'll find %i pieces of coal and %i pieces of watered coal\n",ccnt,wcnt);  
  if(printSolution != dontPrintSolution)
    printSolution_TERMINAL(stdout, true);
  if (OUT) {
    fprintf(OUT, "---\nAnd you'll find %i pieces of coal and %i pieces of watered coal\n",ccnt,wcnt);
    printSolution(OUT, true);
  }
}
  
int main(int argc, char** argv){
  if (argc > 1) {
    freopen(argv[1],"r",stdin);
    printf("Using %s as input.\n", argv[1]);
  }
  if (argc > 2){
    printf("Mirroring output to %s.\n", argv[2]);
    if (strstr(argv[2], ".tex2")) {
      printf("I reckon you want me to produce some graphicless TeX stuff...\n");
      printSolution = dontPrintSolution;
    }
    else if (strstr(argv[2], ".tex")) {
      printf("I reckon you want me to produce some TeX stuff...\n");
      printSolution = printSolution_TEX;
    }
    else if (strstr(argv[2], ".raw")) {
      printf("I reckon you want me to surpress graphics...\n");
      printSolution = dontPrintSolution;
    }
    else
      printSolution = printSolution_TERMINAL;
    OUT = fopen(argv[2],"w");
  }
  else{
    OUT = 0;
    printSolution = printSolution_TERMINAL;
  }

  parseInput(stdin);
  simulateFire(getInitialBurningFields());
}