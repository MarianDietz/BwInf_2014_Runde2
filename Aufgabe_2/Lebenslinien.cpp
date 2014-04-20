#include <cstdio>
#include <vector>
#include <list>
#include <stack>
#include <deque>
#include <algorithm>

std::vector<std::vector<int> > G; //The Graph
int N; //Graphsize

void readInput(){
  scanf("%i",&N);
  G.assign(N,std::vector<int>(N,0));
  
  //Read Graph stored in an Adjancy Matrix
  for(int i = 0; i < N; ++i)
    for(int j= 0; j < N; ++j)
      scanf("%i",&G[i][j]); 
}


//BEGIN lexicographic BFS

std::vector<int> LexBFSOrder(){
 //   std::vector<int> ret(G.size(),-1);
  
  std::list<int> L;
  for(int i = 0; i < (int)G.size(); ++i)
    L.push_back(i);
  std::deque<std::list<int>> classes;
  classes.push_back(L);
    
 //   int cnt = G.size() - 1;
  
  std::deque<int> ret;
  
  while(!classes.empty()){
 //     for(auto i : classes)
 //       for(auto j : i)
 //  	printf("%d ",j + 1);
 //      printf("\n");
    
    int ac = *(classes.begin()->begin());
    classes.begin()->pop_front();
    if(classes.begin()->empty())
      classes.pop_front();
    
    ret.push_front(ac);
    
 //     ret[ac] = cnt--;
    std::deque<std::list<int>> new_classes;
    for(auto i : classes){
      std::list<int> tmp_in, tmp_out;
      
      for(auto j : i)
	if(G[j][ac]) //partion depending on whether neighbor of ac or not
	  tmp_in.push_back(j);
	else
	  tmp_out.push_back(j);
	
      if(!tmp_in.empty())
	new_classes.push_back(tmp_in);
      
      if(!tmp_out.empty())
	new_classes.push_back(tmp_out);
    }
    
    
    classes = new_classes;
  }
  
  return {ret.begin(), ret.end()};
}

//END

//BEGIN Chordality test



 //checks if ordering is a perfect elimination ordering
bool isChordal(std::vector<int> ordering){
  
  std::vector<std::vector<int> > rightNeighs(N, std::vector<int> ()); //Neghbours to the right
  
  std::vector<bool> used(N, false);
  for(int x = 0; x < N; ++x){
    int acnode = ordering[x];
    
    used[acnode] = true;
    
    for(int i = 0; i < N; ++i)
      if(G[i][acnode] && used[i])
	rightNeighs[i].push_back(acnode);
  }
  
  printf("RN | RN(par)\n");
  
  for(int i = 0; i < N; ++i){
    printf("%d:",i);
    
    for(auto j: rightNeighs[i])
      printf(" %d",j);
    printf(" |");
    
    if(rightNeighs[i].size() > 1)
      for(auto j: rightNeighs[rightNeighs[i][0]])
	printf(" %d",j);
    
    printf("\n");
  }
  
  for(int x = 0; x < N; ++x){
    if(rightNeighs[x].size() <= 1)
      continue;
    
   // naive implementation
    for(int i = 1; i < rightNeighs[x].size(); ++i){
      bool existant = false;
      for(auto j : rightNeighs[rightNeighs[x][0]])
	if(j == rightNeighs[x][i]){
	  existant = true;
	  break;
	}
      if(!existant)
	return false;
    }
  }
  
  return true;
}

bool equals(std::vector<int>::iterator a1,std::vector<int>::iterator a2, std::vector<int>::iterator b1, std::vector<int>::iterator b2) {
  if(std::distance(a2,a1) != std::distance(b2,b1))
    return false;
  
  for(a1 != a2 && b1 != b2)
    if(*(a1++) != *(b1) ++)
      return false;
    return true;
}

std::vector<std::pair<std::vector<std::vector<int> >, int > > getCliqueTree(std::vector<int> ordering) {
  std::vector<std::vector<int> > rightNeighs(N, std::vector<int> ()); //Neghbours to the right
  std::vector<std::vector<int> > T(N,std::vector<int>());
  
  std::vector<bool> used(N, false);
  for(int x = 0; x < N; ++x){
    int acnode = ordering[x];
    
    used[acnode] = true;
    
    for(int i = 0; i < N; ++i)
      if(G[i][acnode] && used[i]){
	if(rightNeighs[i].empty())
	  T[acnode].push_back(i);
	rightNeighs[i].push_back(acnode);
      }
  }  
  
  std::stack<std::pair<int,bool> > s;
  std::vector<int> cliquePar(N,-1);
  std::vector<int> clique(N,-1);
  int cCnt = -1;
  for(int i = 0; i< N; ++i)
    if(T[i].empty())
      s.push(pair<int,bool>(i,true)); //push the tree roots
  while(!s.empty()){
    auto ac = s.top(); 
    s.pop();
    
    if(!ac.second) {//foreach vertex except the roots
      std::sort(++rightNeighs[ac.first].begin(),rightNeighs[ac.first].end());
      auto cpy = {rightNeighs[rightNeighs[ac.first][0]].begin(), rightNeighs[rightNeighs[ac.first][0]].end()};
      std::sort(cpy.begin(), cpy.end());
      if(equals(cpy.begin(), cpy.end(), ++rightNeighs[ac.first].begin(),rightNeighs[ac.first].end() )) {
	clique[ac.first] = ++cCnt;
	cliquePar[cCnt] = clique[rightNeighs[ac.first][0]];
      }
      else
	clique[x] = clique[rightNeighs[ac.first][0]];
    }
    
    for(auto i : T[ac.first])
      s.push(pair<int,bool>(i,false));
  }
  
  //reconstruct the tree cN²
  
  std::vector<std::pair<std::vector<std::vector<int> >, int > > T(cCnt, std::pair<std::vector<std::vector<int> >,int>());
  
  for(int i = 0; i < cCnt; ++i){
    T[i].first.assign(N, vector<int>);
    for(int j = 0; j < N; ++j)
      if(clique[j] == i)
	for(int k = 0; k < N; ++k)
	  if(k != j && clique[k] == i)
	    T[i].first[j].push_back(k);
    T[i].second = cliquePar[i];
  }
  return T;
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
  printf("\n");
  if(isChordal(LO))
    printf("The graph is chordal!\n");
  else{
    printf("The graph is not chordal.\n"); //Calculation stops here
    
    //If the graph is not chordal, it has to contain a hole
    //Hole finding starts here therefore.
    
    return 0;
  }
}