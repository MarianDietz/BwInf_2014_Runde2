#include <cstdio>
#include <vector>
#include <list>
#include <stack>
#include <deque>
#include <algorithm>
#include <set>
#include <queue>

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

std::vector<std::pair<std::vector<int>, int > > 
getCliqueTree(std::vector<int> ordering) {
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
  
  std::stack<int> s;
  std::vector<std::set<int> > clique(N,std::set<int>());
  std::set<int> cliques;
  
  for(int i = 0; i< N; ++i)
    if(rightNeighs[i].empty()){
      s.push(i); //push the tree roots
      if(T[i].empty()) { //Is simple node
	clique[i].insert(i);
	cliques.insert(i);
      }
    }
  while(!s.empty()){
    auto ac = s.top(); s.pop();
    
    for(auto child : T[ac]) {
      for(auto i : rightNeighs[child])
	clique[child].insert(i);
      clique[child].insert(child);
      
      auto acParent = rightNeighs[child][0];
      
      if(std::includes(clique[child].begin(),clique[child].end(),
	 clique[acParent].begin(), clique[acParent].end()))
	if(cliques.count(acParent))
	  clique[acParent].erase(clique[acParent].find(acParent));
      
      cliques.insert(child);
      
      if(!T[child].empty())
	s.push(child);
    }
  }
    
  std::vector<std::pair<std::vector<int>, int > > t;
  for(int i = 0; i < N; ++i)
    if(cliques.count(i) && clique[i].count(i)){
      printf("In clique %d:", i);
      for(auto k : clique[i])
	printf(" %d", k);
      printf("\n");
      t.push_back(std::pair<std::vector<int>, int >({clique[i].begin(),clique[i].end()}, rightNeighs[i][0]));
    }
  return t;
}

//END

//BEGIN Interval Graph Reckon'ing

std::deque<std::vector<int>> L; //clique chain

bool isIntervalGraph(std::vector<int> ordering){
  auto T = getCliqueTree(ordering);
  
  printf("Got t\n");
  
  std::list<std::pair<std::vector<int>,int> > l;
  std::list<std::pair<int,int> > treeEdges;
  for(int o = 0; o < T.size(); ++o){
    auto i = T[o];
    l.push_back(std::pair<std::vector<int>,int>(i.first,o));
    treeEdges.push_back(std::pair<int,int>(o,i.second));
  }
  std::list<std::list<std::pair<std::vector<int>, int> > > classes;
  classes.push_back(l);
    
  std::stack<int> pivots;
  
  std::vector<bool> used(N, false);
  
  while(!classes.empty()){
    if(classes.rbegin()->size() == 1) {  //Skip classes containing only 1 element
      L.push_front(classes.rbegin()->rbegin()->first);
      classes.pop_back();
      continue;
    }
    
    std::set<int> C;
    while(!pivots.empty() && used[pivots.top()])
      pivots.pop();
    
    if(pivots.empty()){
      auto Xc = *(classes.rbegin()->rbegin());
      C.insert(Xc.second);
      
      L.push_front(Xc.first);
      classes.rbegin()->pop_back();
      if(classes.rbegin()->empty())
	classes.pop_back();
    }
    else{
      int x = pivots.top(); pivots.pop();
      used[x] = true;
      
      auto Xa = classes.begin(), Xb = classes.begin();
      bool Xaset = false;
      
      for(auto i = classes.begin(); i != classes.end(); ++i)
	for(auto j : *i) {
	  bool contains = false;
	  for(auto k: j.first)
	    if(k == x){
	      contains = true;
	      break;
	    }
	  if(contains)
	    C.insert(j.second);
	  
	  Xb = i;
	  if(!Xaset){
	    Xa = i;
	    Xaset = true;
	  }
	}
	
      //Partion Xa und Xb
      std::list<std::pair<std::vector<int>, int>> tmp_inA, tmp_outA, tmp_inB, tmp_outB;
      
      for(auto i : (*Xa))
	if(C.count(i.second))
	  tmp_inA.push_back(i);
	else
	  tmp_outA.push_back(i);
      for(auto i : (*Xb))
	if(C.count(i.second))
	  tmp_inB.push_back(i);
	else
	  tmp_outB.push_back(i);
      
      (*Xa) = tmp_outA;
      classes.insert(Xa, tmp_inA);
      
      (*Xb) = tmp_outB;
      classes.insert(Xb, tmp_inB);
    }
    
    //Update Pivots
    for(auto i = treeEdges.begin(); i != treeEdges.end(); ++i)
      if(C.count(i->first) != C.count(i->second)){
	treeEdges.erase(i);
	std::set<int> Ci;
	for(auto j : T[i->first].first)
	  Ci.insert(j);
	for(auto j : T[i->second].first)
	  if(Ci.count(j))
	    pivots.push(j);	
      }
  }
  
  printf("After queue\n");
  
  std::vector<bool> alive(N,false), ended (N,false);
  
  for(auto i : L){
    std::vector<bool> seen (N, false);
    for(auto j : i)
      seen[j] = true;
    
    for(int j = 0; j < N; ++j)
      if(ended[j] && seen[j])
	return false;
      else if(seen[j])
	alive[j] = true;
      else if(!seen[j] && alive[j]){
	ended[j] = true;
	alive[j] = false;
      }
  }
  return true;
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
  
  if(isIntervalGraph(LO)){
    
    std::vector<int> start(N, -1), end(N, -1);
    for(int i = 0; i < L.size(); ++i){
      auto ac = L[i];
      for(auto j : ac){
	end[j] = i;
	if(start[j] == -1)
	  start[j] = i;
      }
    }
    
    printf("The graph is an interval graph:\n");
    for(int i = 0; i < N; ++i)
      printf("Node %d: 01.01.%4d - 01.01.%4d\n", i, 1900 + 2 * start[i], 1901 + 2 * start[i]);
  }
  else {
    printf("The graph is not an interval graph.\n");
    
    
    return 0;
  }
}