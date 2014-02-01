#include <cstdio>
#include <vector>
#include <queue>
#include <set>
#include <string>
#include <cstring>
using namespace std;

typedef pair<int,int> PII;

#define FIELDSTATE 	char
#define WALL		0
#define WOODS		1
#define BURNED		2
#define WATERED		4
#define COAL		8

const int oo = (1 << 29);                                   // The infinity

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
FILE* OUT;                                                  // The file to mirror the output to
void (*printField)(FIELDSTATE, int, FILE*);
const char* (*EOL)();
void (*printLegend)(FILE*);

Point getOptimalWaterSpot(vector<Point>& candidates){
  queue<pair<PII,Point> > q;                                // ((distance | color) | Location)
  for(int i= 0; i < candidates.size(); ++i)
    q.push(pair<PII,Point>(PII(0,i),candidates[i]));        // insert all the candidates as start points for the BFS

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

    visited[acPoint.x][acPoint.y].insert(acColor);

    q.pop();
    for(int i= 0; i < 4; ++i){
      int newx = acPoint.x + dir[i][0];
      int newy = acPoint.y + dir[i][1];                     // calculate new field's indexes
      
      if(newx < 0 || newy < 0 || newy >= Forest.height() || newx >= Forest.width())
	continue;                                           // new field is outside the woods
      if (Forest(newx, newy) != WOODS)
	continue;                                           // Field is not of interest
	
      if(visited[newx][newy].count(acColor) == 0)
	if(acDistance + 1 <= shortDis[newx][newy]){
	  shortDis[newx][newy] = acDistance + 1;
	  q.push(pair<PII,Point>(PII(acDistance + 1,acColor),Point(newx,newy)));
	}
    }
  }

  //determine the field to be watered
  vector<int> waterval(candidates.size(),0);

  //Count the number of fields that have an unique fire spot a.k.a. waterval
  for(int i= 0; i< Forest.width(); ++i)
    for(int j= 0; j < Forest.height(); ++j)
      if(visited[i][j].size() == 1)
	waterval[*visited[i][j].begin()]++;
	
  //determine the field of the candidates which has the highest waterval
  int maxv = waterval[0];                                   // maximal value
  int maxi = 0;                                             // index of maximal value
  
  for(int i= 1; i < candidates.size(); ++i)
    if(waterval[i] > maxv){
      maxv = waterval[i];
      maxi = i;
    }
    
  return candidates[maxi];
}

//INPUT
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

//OUTPUT
void printField_TERMINAL(FIELDSTATE acField, int waternum, FILE* f) {
  //The ASCII-magic starts here:
  fprintf(f, "\x1b[s  ");
  if (acField == WALL)
    fprintf(f, "\x1b[u\x1b[37;47m[]");
  if (acField & WOODS)
    fprintf(f, "\x1b[u\x1b[32;42mFO");
  if (acField & BURNED)
    fprintf(f, "\x1b[u\x1b[1;5;31m/\\");
  if (acField & COAL)
    fprintf(f, "\x1b[u\x1b[1;4;5;30m/\\");
  if (acField & WATERED) 
    fprintf(f, "\x1b[u\x1b[46m%02d", waternum);
  fprintf(f, "\x1b[0;39;49m");
}
void printLegend_TERMINAL(FILE* f) {
  fprintf(f, "\nLegend:");
  fprintf(f, "\n\x1b[37;47m[]\x1b[39;49m  ---  WALL");
  fprintf(f, "\n\x1b[32;42mFO\x1b[39;49m  ---  FOREST");
  fprintf(f, "\n\x1b[1;5;31m/\\\x1b[0;39m  ---  BURNED");
  fprintf(f, "\n\x1b[1;4;5;30m/\\\x1b[0;39m  ---  COAL (doubly burned)");
  fprintf(f, "\n\x1b[46m##\x1b[0;39m  ---  WATERED");
  fprintf(f, "\nFields can have more than 1 state.");
}
const char* EOL_TERMINAL() {return "\n"; }

void printField_TEX(FIELDSTATE acField,int waternum, FILE* f){
  if(acField == WALL)
    fprintf(f, "\\colorbox{\\color[gray]{0.5}}{");
  else if(acField & WOODS)
    fprintf(f, "\\colorbox{\\color[rgb]{0,1,0}}{");
  else
    fprintf(f, "{");
     
  if(acField & COAL)
    fprintf(f, "\\color[rgb]{0,0,0}");
  else if(acField & BURNED)
    fprintf(f, "\\color[rgb]{1,0,0}");
  else if(acField == WALL)
    fprintf(f, "\\color[gray]{0.5}");
  else if(acField & WOODS)
    fprintf(f, "\\color[rgb]{0,1,0}");
  else if(acField & WATERED)
    fprintf(f, "\\color[rgb]{0,0,1}");
    
  if(acField & WATERED)
    fprintf(f, "%02d",waternum);
  else if(acField & BURNED)
    fprintf(f, "/\\backslash");
  else if(acField & COAL)
    fprintf(f, "\\underline{/\\backslash}");
  else if(acField == WALL)
    fprintf(f, "[]");
  else if(acField & WOODS)
    fprintf(f, "FO");
  else
    fprintf(f, "--");
  fprintf(f, "}\n");
}
void printLegend_TEX(FILE* f) {
  fprintf(f, "\\\\Legend:");
  fprintf(f, "\\\\\\colorbox{\\color[gray]{0.5}}{\\color[gray]{0.5}[]}  ---  WALL");
  fprintf(f, "\\\\\\colorbox{\\color[rgb]{0,1,0}}{\\color[rgb]{0,1,0}FO}  ---  FOREST");
  fprintf(f, "\\\\{\\color[rgb]{1,0,0}/\\backslash}  ---  BURNED");
  fprintf(f, "\\\\{\\color[rgb]{0,0,0}\\underline{/\\backslash}}  ---  COAL (doubly burned)");
  fprintf(f, "\\\\{\\color[rgb]{0,0,1}##}  ---  WATERED");
  fprintf(f, "\\\\Fields can have more than 1 state.");
}
const char* EOL_TEX() {return "\\\\\n"; }

void printSolution(FILE* f, void (*printField)(FIELDSTATE, int, FILE*), const char* (*EOL)(), void (*printLegend)(FILE*), bool finalOut = true) {	
    
  for(int j= 0; j < Forest.height(); ++j) {
    for(int i= 0; i < Forest.width(); ++i) {
      FIELDSTATE acField = Forest(i, j);
      int waterval = 0;
      if (acField & WATERED) {
	for (int t = 0; t < Solution.size(); ++t)
	  if (Solution[t].x == i && Solution[t].y == j) {
	    waterval = t+1;
	    break;
	  }
      }
      printField(acField, waterval, f);
    }
   
    fprintf(f, "%s", EOL());
  }
  
  if (finalOut) 
    printLegend(f);
  fprintf(f, "%s", EOL());
}

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
  
void simulateFire(const vector<Point>& initiallyBurningFields, bool waterOnlyBurningFields = true) {
  vector<Point> burnedFields = initiallyBurningFields;
  printSolution(stdout, printField_TERMINAL, EOL_TERMINAL, printLegend_TERMINAL, false);
  if (OUT != 0)
    printSolution(OUT, printField, EOL, printLegend, false);
  
  int time = 0;
  while(!burnedFields.empty()) {                            // Simulate as long as there's still fire in the world
    vector<Point> newBurnedFields;                          // The burning fields at the next point of time
    
    //Calculate the new burning fields
    for(size_t i = 0; i < burnedFields.size(); ++i){
      int acx = burnedFields[i].x;
      int acy = burnedFields[i].y;
      
      if(Forest(acx, acy) & WATERED)
	continue;                                           // The field got watered and does not spread fire
      Forest(acx, acy) |= COAL;                             // Field burned down to coal...

      for(int j = 0; j < 4; ++j) {
	int newx = acx + dir[j][0];
	int newy = acy + dir[j][1];
	
	if(newx < 0 || newy < 0 || newy >= Forest.height() || newx >= Forest.width())
	  continue;                                         // new field is outside the woods
	if(Forest(newx, newy) == WOODS){
	  Forest(newx, newy) |= BURNED;                     // Field starts burning
	  newBurnedFields.push_back(Point(newx,newy));
	  
// 	  printf("  From now on burning: (%i|%i)\n",newx,newy);     // log the happenings
	}
      }
    }
    if(newBurnedFields.empty())                             // Nothing to water, all plants happy...
	break;
	
    Point toWater = getOptimalWaterSpot(newBurnedFields);   // Determine the field to water
    Forest(toWater.x, toWater.y) |= WATERED;                // ... and water it
    Solution.push_back(toWater);
    
    burnedFields = newBurnedFields;
    
    //Output / mirror the partial solution
    printf("At time %i: Water spot (%i|%i):\n",++time,toWater.x,toWater.y);
    printSolution(stdout, printField_TERMINAL, EOL_TERMINAL, printLegend_TERMINAL, false);
    
    if (OUT) {
      fprintf(OUT, "At time %i: Water spot (%i|%i):\n",++time,toWater.x,toWater.y);
      printSolution(OUT, printField, EOL, printLegend, false);
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
  printf("And you'll find %i pieces of coal and %i pieces of watered coal:\n",ccnt,wcnt);  
  printSolution(stdout, printField_TERMINAL, EOL_TERMINAL, printLegend_TERMINAL);
  if (OUT) {
    fprintf(OUT, "And you'll find %i pieces of coal and %i pieces of watered coal:\n",ccnt,wcnt);
    printSolution(OUT,  printField, EOL, printLegend);
  }
}
  
int main(int argc, char** argv){
  if (argc > 1) {
    freopen(argv[1],"r",stdin);
    printf("Using %s as input.\n", argv[1]);
  }
  if (argc > 2){
    printf("Mirroring output to %s.\n", argv[2]);
    if (strstr(argv[2], ".tex")) {
      printf("I reckon you want me to produce some TeX stuff...\n");
      EOL = EOL_TEX;
      printField = printField_TEX;
      printLegend = printLegend_TEX;
    }
    else{
      EOL = EOL_TERMINAL;
      printField = printField_TERMINAL;
      printLegend = printLegend_TERMINAL;
    }
    OUT = fopen(argv[2],"w");
  }
  else{
    OUT = 0;
    EOL = EOL_TERMINAL;
    printField = printField_TERMINAL;
    printLegend = printLegend_TERMINAL;
  }

  parseInput(stdin);
  simulateFire(getInitialBurningFields());
}