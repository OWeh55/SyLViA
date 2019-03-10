#ifndef _V23D_POLYGON_H
#define _V23D_POLYGON_H

// "matrix"-size of polygons
constexpr int xySize = 10;

// v shape
constexpr unsigned char xyA[] = { 0, 0, 2, 0, 5, 8, 8, 0, 10, 0,
                                  6, 10, 4, 10, 0, 0
                                };
// x shape
constexpr unsigned char xyB[] = { 0, 0,  1, 0,  4, 4,  7, 0,  8, 0,
                                  8, 1,  5, 5, 5, 10, 3, 10,
                                  3, 5,  0, 1,  0, 0
                                };

// position of marker is upper left corner
constexpr double markerSize = 0.5 ;
constexpr double xPosA = 3 - markerSize / 2;
constexpr double yPosA = 3 - markerSize / 2;

constexpr double xPosB = 7 - markerSize / 2;
constexpr double yPosB = 3 - markerSize / 2;

// distance of planes (A, B) in mm
constexpr double deltaZ = -24;
#endif
