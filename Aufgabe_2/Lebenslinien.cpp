#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

vector<vector<int> > ImpG;

int N; //Graphsize

bool FoundContradiction = false;
void printContradiction(vector<int>& Contradiction){
  FoundContradiction = true;
  printf("Found a Contradiction containing the nodes:\n");
  for(auto i : Contradiction)
    if(i %2 == 0)
      printf("%d\n",(i/2) + 1);
    else
      printf("¬%d\n",(i/2) + 1);
  printf("\n");
}

///SCC - Code
vector<int> dfs_num,dfs_low, S;
vector<bool> visited;
vector<int> inScc;
int dfsNumCounter = 0,SccCounter = 0;

#define UNVISITED -1

bool check(vector<int>& SCC){
  sort(SCC.begin(), SCC.end());
  for(size_t i = 1; i < SCC.size(); ++i)
    if(SCC[i]/2 == SCC[i-1]/2)
      return false;
  return true;
}

void tarjanSCC(int u){
  dfs_low[u] = dfs_num[u] = dfsNumCounter++;
  S.push_back(u);
  visited[u] = true;
  for(auto J = ImpG[u].begin(); J != ImpG[u].end(); ++J){
    if(dfs_num[*J] == UNVISITED)
      tarjanSCC(*J);
    if(visited[*J])
      dfs_low[u] = min(dfs_low[u], dfs_low[*J]);
  }
  
  if(dfs_low[u] == dfs_num[u]){ //Found a SCC
    ++SccCounter;
    vector<int> SCC;
    SCC.clear();
    while(42){
      int v = S.back(); S.pop_back(); visited[v] = false;
      SCC.push_back(v);
      inScc[v] = SccCounter;
      
      if(v%2)
	printf("¬");
      printf("%d ",v/2 + 1);
      if(u == v)
	break;
    }
    printf("\n\n");
    if(!check(SCC))
      printContradiction(SCC);
  }
}

///End SCC - Code

int main(){
  scanf("%i",&N);
  auto g = vector<vector<int> >(N,vector<int>(N,0));
  ImpG.assign(2*N,vector<int>());
  //Read Graph stored in an Adjancy Matrix
  for(int i = 0; i < N; ++i)
    for(int j= 0; j < N; ++j){
      scanf("%i",&g[i][j]); 
      if(i != j)
	if(g[i][j])
	  ImpG[2*i + 1].push_back(2*j);
	else
	  ImpG[2*i].push_back(2*j+1);
    }

    
  dfs_num.assign(2*N,UNVISITED);
  dfs_low.assign(2*N,0);
  visited.assign(2*N,false);
  inScc.assign(2*N,0);
  dfsNumCounter = SccCounter = 0;
  for(int i= 0; i < 2*N; ++i)
    if(dfs_num[i] == UNVISITED)
      tarjanSCC(i);
    
  printf("IMP-GRAPH\n");
  for(int i= 0; i < 2*N; ++i){
    if(i %2 == 1)
      printf("¬");
    printf("%d:",(i/2) + 1);
    for(auto j : ImpG[i])
      if(j %2 == 0)
	printf(" %d",(j/2) + 1);
      else
	printf(" ¬%d",(j/2) + 1);
    printf("\n");
  }   
  if(!FoundContradiction){ //CalculateSolution
    for(int i= 0; i < 2*N; ++i)
      printf("%d %d\n",i,inScc[i]);
  }
}