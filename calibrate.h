#ifndef CALIBRATE_H
#define CALIBRATE_H

void findMarker(Image& tGray, Image& tMark,
                vector<Contur>& squares, Trafo& tA, Trafo& tB);

constexpr double rasterMM = 22.4;

extern bool verbose;
extern int debug;

extern int localSegSize;
extern int localSegLevel;

#endif
