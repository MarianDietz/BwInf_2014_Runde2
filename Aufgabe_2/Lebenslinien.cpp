#include <cstdio>
#include <vector>
#include <list>
#include <stack>
#include <deque>
#include <algorithm>
#include <iterator>
#include <set>
#include <queue>

std::vector<std::vector<int> > G; //The Graph
int N; //Graphsize

void readInput(){
  std::printf("The graphsize.\n");
  scanf("%i",&N);
  G.assign(N,std::vector<int>(N,0));
  
  std::printf("%d lines à %d space seperated integers;\nthe graph as adjancy matrix.\n",N,N);
  //Read Graph stored in an Adjancy Matrix
  for(int i = 0; i < N; ++i)
    for(int j= 0; j < N; ++j)
      scanf("%i",&G[i][j]); 
}


//BEGIN lexicographic BFS

std::vector<int> LexBFSOrder(){
  std::list<int> L;
  for(int i = 0; i < (int)G.size(); ++i)
    L.push_back(i);
  std::deque<std::list<int>> classes;
  classes.push_back(L);
  std::deque<int> ret;
  
  while(!classes.empty()){
    int ac = *(classes.begin()->begin());
    classes.begin()->pop_front();
    if(classes.begin()->empty())
      classes.pop_front();
    
    ret.push_front(ac);
    
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
getCliqueTree(std::vector<int> ordering, bool printCliques = true) {
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
  std::vector<int> num(N, -1);
  int cnt = 0;
  for(int i = 0; i < N; ++i)
    if(cliques.count(i) && clique[i].count(i))
      num[i] = cnt++;
  for(int i = 0; i < N; ++i)
    if(cliques.count(i) && clique[i].count(i)){
      
      if(printCliques){
	std::printf("In clique %d:", i);
	for(auto k : clique[i])
	  std::printf(" %d", k);
	std::printf("\n");
      }
      int par = (rightNeighs[i].empty() ? -1 : num[rightNeighs[i][0]]);
      t.push_back(std::pair<std::vector<int>, int >({clique[i].begin(),clique[i].end()}, par));
    }
  return t;
}

//END

//BEGIN Interval Graph Reckon'ing

std::deque<std::vector<int>> L; //clique chain

bool isIntervalGraph(std::vector<int> ordering, bool printOut = true){
  auto T = getCliqueTree(ordering, printOut);
  
  std::list<std::vector<int> > l;
  for(size_t o = 0; o < T.size(); ++o)
    l.push_back(T[o].first);
  
  std::list<std::list< std::vector<int> > > classes;
  classes.push_back(l);
    
  std::vector<bool> used(N, false);
  
  L.clear();
  
  while(!classes.empty()){
  
    if(printOut){
      for(auto i = classes.begin(); i != classes.end(); ++i){
	if(i != classes.begin())
	  printf(" |");
	for(auto j = i->begin(); j != i->end(); ++j){
	  if(j != i->begin())
	    printf(",");
	  for(auto k : *j)
	    printf(" %d",k);
	}
      }
      printf("\n");
    }
    
    
    auto LST = *(classes.rbegin()->rbegin());
    L.push_front(LST);
    classes.rbegin()->pop_back();
    
    
    std::sort(LST.begin(),LST.end());
    
    std::list< std::vector<int> > tmp_in, tmp_out;
    std::vector< int >tmp;
    for(auto I = classes.rbegin()->begin(); I != classes.rbegin()->end(); ++I){
      tmp.clear();
      
      std::sort(I->begin(),I->end());
      std::set_intersection(LST.begin(), LST.end(),
			I->begin(), I->end(),
			std::back_inserter(tmp));
            
      if(tmp.empty())
	tmp_out.push_back(*I);
      else
	tmp_in.push_back(*I);
      
    }
    classes.pop_back();
    if(!tmp_out.empty())
      classes.push_back(tmp_out);
    if(!tmp_in.empty())
      classes.push_back(tmp_in);
  } 
  
  std::vector<bool> alive(N,false), ended (N,false);
  
  if(printOut)
    printf("END\n");
  for(auto i : L){

    if(printOut) {
      for(auto j : i)
	printf(" %d",j);
      printf("\n");
    }
    
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

//determines smallest subgraph that is not an interval graph
std::vector<int> smallestFailingSubgraph(){
  auto G2 = G;
  int N2 = N;
  for(N = 4; N <= N2; ++N){ //try all size i subsets of the nodes
    std::vector<bool> btm(N2,false);
    for(int i = N2-1; i >= N2 - N; --i)
      btm[i] = true;
    do {
      G.assign(N,std::vector<int>(N,0));
      int i = 0, j = 0;
      
      for(int k = 0; k < N2; ++k)
	if(btm[k]){
	  j = 0;
	  for(int l = 0; l < N2; ++l)
	    if(btm[l])
	      G[i][j++] = G2[k][l];
	  ++i;
	}
      auto LO = LexBFSOrder();
      if(!isChordal(LO) || !isIntervalGraph(LO, false)) { //Found smallest subgraph, that is not chordal
	
	std::vector<int> ret;
	
	for(int k = 0; k < N2; ++k)
	  if(btm[k])
	    ret.push_back(k);
	return ret;
      }
      
    }while(std::next_permutation(btm.begin(),btm.end()));
  }
  
  return std::vector<int>();
}

int main(){
  readInput();
  auto LO = LexBFSOrder(); 
  if(isChordal(LO))
    std::printf("The graph is chordal!\n");
  else{
    std::printf("The graph is not chordal.\n"); //Calculation stops here
   
    std::printf("Smallest subgraph that is not chordal:\n");
    for(auto i : smallestFailingSubgraph())
      std::printf("Node %d\n",i);
    
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
    
    std::printf("The graph is an interval graph:\n");
    for(int i = 0; i < N; ++i)
      std::printf("Node %d: 25.09.%4d - 16.10.%4d\n", i, 1900 + 2 * start[i], 1901 + 2 * end[i]);
  }
  else {
    std::printf("The graph is not an interval graph.\n");
    
    std::printf("Smallest subgraph that is not an interval graph:\n");
    for(auto i : smallestFailingSubgraph())
      std::printf("Node %d\n",i);
    
    return 0;
  }
}