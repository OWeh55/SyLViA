#include <image.h>

const int dpi = 300;
const int dpm = dpi / 25;

// querformat
const int xMM = 270;
const int yMM = 180;

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
  m.write("muster.bmp");
  GetChar();
  return OK;
}
