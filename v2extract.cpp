#include "v23d.h"

void getImage(const std::vector<ice::Image>& cv,
              double idx1, double idx2, ImageD& sp)
{
  sp.set(0);
  int n = 0;
  int i1 = RoundInt(idx1);
  int i2 = RoundInt(idx2);
  WindowWalker ww(sp);
  for (int i = i1; i <= i2; i++)
    {
      if (i > (idx1 + 0.5) && i < (idx2 - 0.5))
        {
          n++;
          for (ww.init(); !ww.ready(); ww.next())
            {
              double v = cv[i].getPixel(ww);
              sp.setPixel(ww, sp.getPixel(ww) + v);
            }
          /*
          Show(GRAY,cv[i]);
          cout << i << " ";
          GetChar();
          Show(OFF,cv[i]);
          */
        }
    }
  if (n == 0)
    throw IceException("getImage", "empty image range");
  for (ww.init(); !ww.ready(); ww.next())
    sp.setPixel(ww, sp.getPixel(ww) / n);
  //  cout << endl;
}

void getPattern(const std::vector<ice::Image>& cv,
                double idx1, double idx2, vector<ImageD>& sp)
{
  double dIdx = (idx2 - idx1) / sequenceLength;
  ImageD pos;
  pos.create(xSize, ySize);
  ImageD neg;
  neg.create(xSize, ySize);
  for (int i = 0; i < sp.size(); i++)
    {
      double i1 = idx1 + (1 + i * 2) * dIdx;
      double i2 = idx1 + (1 + i * 2 + 1) * dIdx;
      double i3 = idx1 + (1 + i * 2 + 2) * dIdx;
      //cout << "pattern " << i << ": " << i1 << " ... " << i2 << " ... " << i3 << endl;
      getImage(cv, i1, i2, pos);
      getImage(cv, i2, i3, neg);
      WindowWalker ww(cv[i]);
      for (ww.init(); !ww.ready(); ww.next())
        {
          double v = pos.getPixel(ww) - neg.getPixel(ww);
          sp[i].setPixel(ww, v);
        }
      sp[i].adaptLimits();
    }
}

void extractPattern(const std::vector<ice::Image>& cv,
                    vector<double>& sb,
                    std::vector<std::vector<ImageD>>& extractedPattern)
{
  int nPattern = sequenceDescription.length();
  extractedPattern.clear();
  for (int i = 0; i < sb.size() - 1; i++)
    {
      vector<ImageD> sp(nPattern);
      for (int i = 0; i < nPattern; i++)
        {
          sp[i].create(xSize, ySize, -100, 100);
          if (debug & 4)
            {
              Show(GRAY, sp[i]);
              Zoom(sp[i], -4);
            }
        }
      getPattern(cv, sb[i], sb[i + 1], sp);
      extractedPattern.push_back(sp);
      if (debug & 4)
        {
          GetChar();
          for (int i = 0; i < nPattern; i++)
            Show(OFF, sp[i]);
        }
    }
}
