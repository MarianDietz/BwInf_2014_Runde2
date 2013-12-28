#include <cstdio>
#include <vector>
#include <queue>
#include <set>
using namespace std;

typedef pair<int,int> PII;

const int MAXN = 100; //max size of the field
enum FIELDSTATE{
  WALL = 0,
  WOODS = 1,
  BURNED = 2,
  WATERED = 4,
  COAL = 8
} field[MAXN][MAXN];

const int oo = (1 << 29);

int N; //fieldsize

struct Point {
public:
  int x, y; 
  Point(int _x,int _y) : x(_x), y(_y) { }
};
int dir[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

Point getOptimalWaterSpot(vector<Point>& points){
  queue<pair<PII,Point> > q;//((distance | color) | Location)
  for(int i= 0; i < points.size(); ++i)
    q.push(pair<PII,Point>(PII(0,i),points[i]));
  
  vector<vector<set<int> > > visited(N,vector<set<int> >(N));
  vector<vector<int> > shortDis(N,vector<int>(N,oo));
  
  while(!q.empty()){
    pair<PII,Point> ac = q.top();
    Point acPoint = ac.second;
    int acDistance = ac.first.first;
    int acColor = ac.first.second;
    
    visited[acPoint.x][acPoint.y].insert(acColor);
    
    q.pop();
    for(int i= 0; i < 4; ++i){
      int newx = acPoint.x + dir[i][0], newy = acPoint.y + dir[i][1];
      if(newx < 0 || newy < 0 || newx >= N || newy >= N)
	continue;
      if (field[newx][newy] != WOODS)
	continue; //Field is not of interest
      if(!visited[newx][newy].contains(acColor)) 
	if(acDistance + 1 <= shortDis[newx][newy]){
	  shortDis[newx][newy] = acDistance + 1;
	  q.push(pair<PII,Point>(PII(acDistance + 1,acColor),Point(newx,newy)));
	}
    }
  }
  
  vector<int> waterval(N,0);
  
  for(int i= 0; i< N; ++i)
    for(int j= 0; j < N; ++j)
      if(visited[i][j].size() == 1)
	waterval[*visited[i][j].begin()]++;
  int maxv = waterval[0],maxi = 0;
  for(int i= 1; i < N; ++i)
    if(waterval[i] > maxv){
      maxv = waterval[i];
      maxi = i;
    }
  return points[maxi];
}

int main(){
//   freopen("test.in","r",stdin);
//   freopen("test.out","w",stdout);
  
  vector<Point> burnedFields; //Actuel burning fields
  
  //read input
  scanf("%i",&N);
  for(int i = 0; i < N; ++i){
    for(int j= 0; j < N; ++j){
      char c;
      scanf("%c",&c);
      field[i][j] = (FIELDSTATE) c;
      if(field[i][j] == BURNED)
	burnedFields.push_back(Point(i,j));
    }
    scanf("\n");
  }
  //end of read input
  
  int time = 0;
  while(!burnedFields.empty()){
    vector<Point> newBurnedFields;
    for(size_t i = 0; i < burnedFields.size(); ++i){
      int acx = burnedFields[i].x, acy = burnedFields[i].y;
      if(field[acx][acy] & WATERED)
	continue;
      field[acx][acy] |= COAL;
      
      for(int j = 0; j < 4; ++j)
	if(field[acx + dir[j][0]][acy+dir[j][1]] == WOODS){
	  field[acx + dir[j][0]][acy+dir[j][1]] = BURNED; //Field starts burning
	  newBurnedFields.push_back(Point(acx + dir[j][0],acy + dir[j][1]));
	}
    }
    Point toWater = getOptimalWaterSpot(newBurnedFields);
    field[toWater.x][toWater.y] |= WATERED;
    
    printf("At time %i: Water spot (%i|%i).\n",++time,toWater.x,toWater.y);
    
    burnedFields = newBurnedFields;
  }  
}