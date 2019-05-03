#ifndef TO3D_H
#define TO3D_H

#include "image.h"

class SPixel
{
public:
  SPixel():
    color(0), phase(0), position(0, 0, 0), valid(false), selected(false), nr(-1)
  {
  }
  ColorValue color;
  double phase;
  Vector3d position;
  bool valid;
  bool selected;
  int nr;
};

typedef matrix<SPixel> SWorld;

class SelectBox
{
public:
  SelectBox(double xMin, double xMax,
            double yMin, double yMax,
            double zMin, double zMax):
    xMin(xMin), xMax(xMax),
    yMin(yMin), yMax(yMax),
    zMin(zMin), zMax(zMax)
  {
  };
  bool inRange(const Vector3d& xyz)
  {
    return  xyz.x >= xMin && xyz.x <= xMax &&
            xyz.y >= yMin && xyz.y <= yMax &&
            xyz.z >= zMin && xyz.z <= zMax;
  }
private:
  double xMin;
  double xMax;
  double yMin;
  double yMax;
  double zMin;
  double zMax;
};

// in structure.cpp
void smooth(SWorld& world, int dist);

extern bool verbose;
#endif
