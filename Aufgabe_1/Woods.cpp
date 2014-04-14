#include <vector>
#include <cstdio>

#include "Woods.h"

FIELDSTATE ERROR = -1;

Woods::Woods(int width, int height) : Width(width), Height(height) {
  Fields.assign(height, std::vector<FIELDSTATE>(width, 0));
}

int Woods::width() const { return this->Width; }
int Woods::height() const { return this->Height; }

FIELDSTATE& Woods::operator() (int x, int y) {
  if (x < 0 || y < 0 || x >= width() || y >= height()){
    printf("OUT OF BOUNDS 1");
    return ERROR;
  }
  return this->Fields[y][x];
}
FIELDSTATE Woods::operator() (int x,  int y) const {
  if (x < 0 || y < 0 || x >= width() || y >= height()){
    printf("OUT OF BOUNDS 2");
    return ERROR;
  }
  return this->Fields[y][x];
}

bool Woods::operator==(const Woods& o) const{
  if(o.width() != width() || o.height() != height())
    return false;
  
  for(int i = 0; i< width(); ++i)
    for(int j = 0; j < height(); ++j)
      if(o(i,j) != Fields[i][j])
	return false;
  return true;
  
  return (string() == o.string());
}

bool Woods::operator <(const Woods& o) const {
  if(std::min(o.width() , o.height()) < std::min(width(), height()))
    return true;
  if(std::max(o.width() , o.height()) < std::max(width(), height()))
    return true;
  if(std::min(o.width() , o.height()) > std::min(width(), height()))
    return false;
  if(std::max(o.width() , o.height()) > std::max(width(), height()))
    return false;
  
  for(int i = 0; i< width(); ++i)
    for(int j = 0; j < height(); ++j)
      if(o(i,j) <= Fields[i][j])
	return false;
  return true;
}

extern int dir[4][2];
std::string Woods::string() const {
  std::string ret;
  
  for(int i = 0; i< width(); ++i)
    for(int j = 0; j < height(); ++j){
      u16 andMask = (BURNABLE | BURNED | COAL);
      
      for(int k = 0; k < 4; ++k){
	int nx = i + dir[i][0], ny = j + dir[i][1];
	
	if(nx < 0 || ny < 0 || nx >= width() || ny >= height())
	  continue;
	if(!(Fields[nx][ny] & (COAL | BURNED | WATERED))){
	  andMask |= (WATERED);//Only watered fields at the border of the fire are of interest
	  break;
	}
      }
      
      ret += (char) (((Fields[i][j] & andMask)) + 'A');
    }
    
  return ret;
}

int Woods::cnt() const { //counts the burningfields 
  int ret = 0;
  for(int i = 0; i< width(); ++i)
    for(int j = 0; j < height(); ++j)
      if(Fields[i][j] & BURNED && !(Fields[i][j] & COAL))
	ret++;
  return ret;
}

int Woods::cnt2() const { //counts the burningfields 
  int ret = 0;
  for(int i = 0; i< width(); ++i)
    for(int j = 0; j < height(); ++j)
      if(Fields[i][j] & BURNED)
	ret++;
  return ret;
}