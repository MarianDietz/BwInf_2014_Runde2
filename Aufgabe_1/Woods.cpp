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