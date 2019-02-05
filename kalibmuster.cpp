#include <image.h>
#include "v2polygon.h"

const int dpi = 300;
const int dpm = dpi / 25;

// querformat
const int xMM = 270;
const int yMM = 180;

vector<Point> makePointList(const unsigned char* xy)
{
  Point start(xy[0], xy[1]);
  vector<Point> pl;
  pl.push_back(start);
  int idx = 2;
  Point p(xy[idx], xy[idx + 1]);
  while (p != start)
    {
      pl.push_back(p);
      idx += 2;
      p = Point(xy[idx], xy[idx + 1]);
    }
  return pl;
}

void drawMarker(Image& m, vector<Point> points, Point center, int size)
{
  double factor = (double)size / xySize;
  Point loPoint = center - Point(size / 2, size / 2);
  vector<Point> pl(points.size());
  for (int i = 0; i < points.size(); i++)
    {
      pl[i] = points[i] * factor + loPoint;
    }
  Polygon polygon(pl);
  draw(polygon, m, m.maxval, m.maxval);
}

int main(int argc, char** argv)
{
  Image m;
  m.create(270 * dpm, 180 * dpm, 255);
  m.set(0);
  Show(ON, m);
  int rasterMM = 20;
  int squareMM = 5;
  int nX = xMM / rasterMM;
  int nY = yMM / rasterMM;
  int rasterP = rasterMM * dpm;
  int squareP = squareMM * dpm;
  int border = (rasterP - squareP) / 2;
  for (int yi = 0; yi < nY; yi++)
    {
      int y0 = yi * rasterP + border;
      for (int xi = 0; xi < nX; xi++)
        {
          int x0 = xi * rasterP + border;
          for (int y = 0; y < squareP; y++)
            for (int x = 0; x < squareP; x++)
              m.setPixel(x + x0, y + y0, 255);
        }
    }

  int markerSize = 0.5 * rasterP;

  drawMarker(m, makePointList(xyA), Point(3 * rasterP, 3 * rasterP), markerSize);
  drawMarker(m, makePointList(xyB), Point(7 * rasterP, 3 * rasterP), markerSize);

  m.write("muster.bmp");
  GetChar();
  return OK;
}
