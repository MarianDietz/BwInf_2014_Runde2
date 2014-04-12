#include <cstdio>
#include <vector>
#include <list>
#include <stack>
#include <deque>

std::vector<std::vector<int> > G; //The Graph

void readInput(){
  int N; scanf("%i",&N);
  G.assign(N,std::vector<int>(N,0));
  
  //Read Graph stored in an Adjancy Matrix
  for(int i = 0; i < N; ++i)
    for(int j= 0; j < N; ++j)
      scanf("%i",&G[i][j]); 
}


//BEGIN lexicographic BFS

std::vector<int> LexBFSOrder(){
  std::vector<int> ret(G.size(),-1);
  
  std::list<int> L;
  for(int i = 0; i < (int)G.size(); ++i)
    L.push_back(i);
  std::deque<std::list<int>> classes;
  classes.push_back(L);
    
  int cnt = G.size() - 1;
  while(!classes.empty()){
    for(auto i : classes)
      for(auto j : i)
	printf("%d ",j + 1);
    printf("\n");
    
    int ac = *(classes.rbegin()->rbegin());
    classes.rbegin()->pop_back();
    if(classes.rbegin()->empty())
      classes.pop_back();
    
    ret[ac] = cnt--;
    std::deque<std::list<int>> new_classes;
    for(auto i : classes){
      std::list<int> tmp_in, tmp_out;
      
      for(auto j : i)
	if(G[j][ac]) //partion depending on whether neighbor of ac or not
	  tmp_in.push_back(j);
	else
	  tmp_out.push_back(j);
	
      if(!tmp_out.empty())
	new_classes.push_back(tmp_out);
      if(!tmp_in.empty())
	new_classes.push_back(tmp_in);
      
    }
    
    
    classes = new_classes;
  }
  
  return ret;
}

//returns a pair of a collection of maximal cliques and a collection of edges between
//those maximal cliques in a clique tree
std::pair< std::vector<std::vector<int> >, std::vector<std::pair<int,int> > >
LexBFS(){
  
}

//END

//BEGIN Chordality test

//checks if ordering is a perfect elimination ordering
bool isChordal(std::vector<int> ordering){
  
  
  
  return true;
}

//END

//BEGIN Interval Graph Reckon'ing

std::list<std::vector<int>> L; //clique chain

bool isIntervalGraph(){
//   auto T = LexBFS();
//   
//   auto X = T.first;
//   
//   std::list<int> L;
//   for(int i = 0; i < (int)X.size(); ++i)
//     L.push_back(i);
//   std::vector<std::list<int>> classes;
//   classes.push_back(L);
//     
//   stack<int> pivots;
//   
//   while(!classes.empty()){
//     int ac = *(classes.rbegin()->rbegin());
//     classes.rbegin()->pop_back();
//     if(classes.rbegin()->empty())
//       classes.pop_back();
//     
//     ret[ac] = cnt--;
//     std::vector<std::list<int>> new_classes;
//     for(auto i : classes){
//       std::list<int> tmp_in, tmp_out;
//       
//       for(auto j : i)
// 	if(G[j][ac]) //partion depending on whether neighbor of ac or not
// 	  tmp_in.push_back(j);
// 	else
// 	  tmp_out.push_back(j);
// 	
//       if(!tmp_in.empty())
// 	new_classes.push_back(tmp_in);
//       if(!tmp_out.empty())
// 	new_classes.push_back(tmp_out);
//       
//     }
//     classes = new_classes;
//   }
  return false;
}

//END

int main(){
  readInput();
  auto LO = LexBFSOrder();
  for(auto i : LO)
    printf("%d ",i + 1);
}