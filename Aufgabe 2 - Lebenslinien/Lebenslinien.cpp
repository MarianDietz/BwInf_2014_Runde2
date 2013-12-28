#include <cstdio>
#include <vector>
#include <map>

using namespace std;

const int MAXV = 1000; //max. 1000 Vertices per Graph

vector<int> G[MAXV];

int N; //Graphsize

///SCC - Code


///End SCC - Code

inline int getIndex(int a, int b){ //getIndex returns unique id for ordered pair (a;b); max N^2
  return (N-1)*a + b + 1;
}
inline pair<int,int> getIndex(int n){
  if(N <= 1)
    return pair<int,int>(0,0);
  for(int i= 0; i < N; ++i)
    if(n == getIndex((n-1-i) / (N-1),i))
      return pair<int,int>((n-1-i) / (N-1),i);
}

int main(){
  scanf("%i",&N);
  int g[MAXV][MAXV];
  //Read Graph stored in an Adjancy Matrix
  for(int i = 0; i < N; ++i)
    for(int j= 0; j < N; ++j){
      scanf("%i",&g[i][j]); 
      //Construct Implication Graph
      
      
    }
}