#include <image.h>
#include "v2polygon.h"

// resolution
const int dpi = 300;
const int dpm = dpi / 25;

// size (in mm)
const int xMM = 270;
const int yMM = 180;

vector<Point> makePointList(const unsigned char* xy)
{
  // transform c array xy to vector of points
  IPoint start(xy[0], xy[1]);
  vector<Point> pl;
  pl.push_back(start);
  int idx = 2;
  IPoint p(xy[idx], xy[idx + 1]);

  // last point has same values as first point, stop here
  while (p != start)
    {
      Point dp(p);
      // integer values in xy are interpreted as relative to xySize
      dp *= 1.0 / xySize;
      pl.push_back(dp);
      idx += 2;
      // get next point
      p = IPoint(xy[idx], xy[idx + 1]);
    }
  return pl;
}

void drawMarker(Image& m, const vector<Point> &points,
		Point luPoint, int size)
{
  vector<Point> pl(points.size());
  // transform relative values in points to
  // coordinates in image m
  for (int i = 0; i < points.size(); i++)
    {
      pl[i] = points[i] * size + luPoint;
    }
  Polygon polygon(pl);
  draw(polygon, m, m.maxval, m.maxval);
}

int main(int argc, char** argv)
{
  Image m;
  // paper size xMM,yMM, resolution dpm
  m.create(xMM * dpm, yMM * dpm, 255);
  m.set(0);
  Show(ON, m);
  // raster of markers
  int rasterMM = 20;
  // size of square
  int squareMM = 8;
  // size of hole
  int holeMM = 5;
  // number of rows and columns
  int nX = xMM / rasterMM;
  int nY = yMM / rasterMM;
  // measures in pixel
  int rasterP = rasterMM * dpm;
  int squareP = squareMM * dpm;
  int holeP = holeMM * dpm;
  int border = (rasterP - squareP) / 2;
  int borderH = (rasterP - holeP) / 2;
  for (int yi = 0; yi < nY; yi++)
    {
      int y0 = yi * rasterP + border;
      int y1 = yi * rasterP + borderH;
      for (int xi = 0; xi < nX; xi++)
        {
          int x0 = xi * rasterP + border;
          int x1 = xi * rasterP + borderH;
	  // black square
          for (int y = 0; y < squareP; y++)
            for (int x = 0; x < squareP; x++)
              m.setPixel(x + x0, y + y0, 255);
	  // white hole
          for (int y = 0; y < holeP; y++)
            for (int x = 0; x < holeP; x++)
              m.setPixel(x + x1, y + y1, 0);
        }
    }

  int markerSizeP =  markerSize * rasterP;

  // marker V
  drawMarker(m, makePointList(xyA),
             Point(xPosA * rasterP, yPosA * rasterP), markerSizeP);
  // marker Y
  drawMarker(m, makePointList(xyB),
             Point(xPosB * rasterP, yPosB * rasterP), markerSizeP);

  m.write("muster.pgm");
  GetChar();
  return OK;
}
