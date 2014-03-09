#include <vector>

#define FIELDSTATE 	char
#define EMPTY		0
#define BURNABLE	1
#define BURNED		2
#define WATERED		4
#define COAL		8

class Woods{
  private:
  int Width, Height;
  std::vector<std::vector<FIELDSTATE> > Fields;
  
public:
  Woods(int width, int height);
  
  int width() const;
  int height() const;
  
  FIELDSTATE& operator() (int x, int y);
  FIELDSTATE operator() (int x,  int y) const;
};