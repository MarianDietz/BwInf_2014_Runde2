#include <cstdio>
#include <vector>

#include "Woods.h"

typedef std::pair<int,int> PII;

const int oo = (1 << 29);                                  // The infinity
Woods Forest(0, 0);

struct Point {
public:
  int x, y; 
  Point(int _x,int _y) : x(_x), y(_y) { }
  };
int dir[4][2] = {{1,0},{0,1},{-1,0},{0,-1}};

std::vector<Point> Solution;
std::FILE* OUT;                                                 // The file to mirror the output to
void (*printSolution)(std::FILE*, bool);

//BEGIN OF INPUT
void parseInput(std::FILE* f) {
  int acFieldWidth, acFieldHeight;
  std::fscanf(f, "%i %i\n",&acFieldWidth, &acFieldHeight);
  
  Forest = Woods(acFieldWidth, acFieldHeight);

  for(int i = 0; i < acFieldHeight; ++i){
    for(int j= 0; j < acFieldWidth; ++j){
      char c;
      std::fscanf(f, "%c",&c);
      c -= '0';
      Forest(j, i) = (FIELDSTATE) c;
    }
    if(i < acFieldHeight-1)
      std::fscanf(f, "\n");
  }
}
//END OF INPUT
//BEGIN OF OUTPUT
void printSolution_TEX(std::FILE* f, bool finalOut) {	
  std::fprintf(f, "\\\\\n");
  
  std::fprintf(f, "\\begin{tikzpicture}\n");
  std::fprintf(f, "\\tikzset{square matrix/.style={\n");
  std::fprintf(f, "matrix of nodes,\n");
  std::fprintf(f, "column sep=-\\pgflinewidth, row sep=-\\pgflinewidth,\n");
  std::fprintf(f, "nodes={draw,\n");
  std::fprintf(f, "minimum height=#1,\n");
  std::fprintf(f, "anchor=center,\n");
  std::fprintf(f, "text width=#1,\n");
  std::fprintf(f, "align=center,\n");
  std::fprintf(f, "inner sep=0pt\n");
  std::fprintf(f, "},\n");
  std::fprintf(f, "},\n");
  std::fprintf(f, "square matrix/.default=1.2cm\n");
  std::fprintf(f, "}\n");
  
  std::fprintf(f, "\\matrix[square matrix=1.4em] {\n");
  for(int j= 0; j < Forest.height(); ++j) {
    for(int i= 0; i < Forest.width(); ++i) {
      if(i)
	std::fprintf(f," &");
      
        FIELDSTATE acField = Forest(i, j);
	if(acField == EMPTY)
	  std::fprintf(f, "|[fill=white]|");
	else if(acField & WATERED)
	  std::fprintf(f, "|[fill=cyan]|");
	else if(acField & BURNABLE)
	  std::fprintf(f, "|[fill=green]|");
	  
	if(acField & COAL)
	  std::fprintf(f, "\\color[rgb]{0,0,0}");
	else if(acField & BURNED)
	  std::fprintf(f, "\\color[rgb]{1,0,0}");
	else if(acField == EMPTY)
	  std::fprintf(f, "\\color[gray]{0.5}");
	else if(acField & BURNABLE)
	  std::fprintf(f, "\\color[gray]{0.75}");
	  
	if(acField & WATERED){
	  for (int t = 0; t < Solution.size(); ++t)
	    if (Solution[t].x == i && Solution[t].y == j) {
	      std::fprintf(f, "\\textbf{\\texttt{%02d}}",t+1);
	      break;
	    }	  
	}
	else if(acField & COAL)
	  std::fprintf(f, "\\textbf{\\texttt{CO}}");
	else if(acField & BURNED)
	  std::fprintf(f, "\\textbf{\\texttt{BU}}");
	else if(acField == EMPTY)
	  std::fprintf(f, "\\texttt{WA}");
	else if(acField & BURNABLE)
	  std::fprintf(f, "\\texttt{FO}");
	else
	  std::fprintf(f, "\\phantom{AA}");
      std::fprintf(f, "%%\n");
    }
   
    std::fprintf(f, "\\\\\n");
  }
  
  std::fprintf(f, "};\n\\end{tikzpicture}\\\\\n");
  
  if(finalOut){
    std::fprintf(f, "\\\\\nExplanation:");
    std::fprintf(f, "\\\\\n\\colorbox{white}{\\color[gray]{0.5}WA}  ---  EMPTY");
    std::fprintf(f, "\\\\\n\\colorbox{green}{\\color[gray]{0.5}FO}  ---  BURNABLE");
    std::fprintf(f, "\\\\\n\\colorbox{white}{\\color[rgb]{1,0,0}\\textbf{BU}}  ---  BURNED");
    std::fprintf(f, "\\\\\n\\colorbox{white}{\\color[rgb]{0,0,0}\\textbf{CO}}  ---  COAL (doubly burned)");
    std::fprintf(f, "\\\\\n\\colorbox{cyan}{\\#\\#}  ---  WATERED at time \\#\\#");
    std::fprintf(f, "\\\\\nFields can have more than 1 state.");
  }
}

void printSolution_TERMINAL(std::FILE* f, bool finalOut) {	
  fprintf(f, "\n");
  //The ASCII-magic starts here:
  for(int j= 0; j < Forest.height(); ++j) {
    for(int i= 0; i < Forest.width(); ++i) {
      FIELDSTATE acField = Forest(i, j);
      int waterval = 0;
      
      fprintf(f, "\x1b[s  ");
      if (acField == EMPTY)
	std::fprintf(f, "\x1b[u\x1b[37;47mWA");
      if (acField & BURNABLE)
	std::fprintf(f, "\x1b[u\x1b[32;42mFO");
      if (acField & BURNED)
	std::fprintf(f, "\x1b[u\x1b[1;5;31m/\\");
      if (acField & COAL)
	std::fprintf(f, "\x1b[u\x1b[1;4;5;30m/\\");
      if (acField & WATERED)
	for (int t = 0; t < Solution.size(); ++t)
	  if (Solution[t].x == i && Solution[t].y == j) {
	    std::fprintf(f, "\x1b[u\x1b[46m%02d", t+1);
	    break;
	  }
      std::fprintf(f, "\x1b[0;39;49m");
    }
   
    std::fprintf(f, "\n");
  }
  
  if (finalOut) {// An Explanation shall be printed
    std::fprintf(f, "\nExplanation:");
    std::fprintf(f, "\n\x1b[37;47mWA\x1b[39;49m  ---  EMPTY");
    std::fprintf(f, "\n\x1b[32;42mFO\x1b[39;49m  ---  BURNABLE");
    std::fprintf(f, "\n\x1b[1;5;31m/\\\x1b[0;39m  ---  BURNED");
    std::fprintf(f, "\n\x1b[1;4;5;30m/\\\x1b[0;39m  ---  COAL (doubly burned)");
    std::fprintf(f, "\n\x1b[46m##\x1b[0;39m  ---  WATERED at time ##");
    std::fprintf(f, "\nFields can have more than 1 state.");
  }
  std::fprintf(f, "\n");
}

void dontPrintSolution(std::FILE* f,bool finalOut) { return; }
//END OF OUTPUT
