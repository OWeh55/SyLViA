#include <fstream>
#include <iostream>

#include "v23d.h"

using namespace std;

void readImages(const string& fn,
                vector<Image>& ivector,
                int& fps,
                int first, int last,
                int colorMode)
{
  int xo, yo, mv;

  VideoFile input(fn);
  input.getPara(xo, yo, mv, fps);

  if (verbose)
    {
      cout << "video " << fn << " with " << xo << "x" << yo ;
      cout << ", " << fps << " frames per second" << endl;
    }

  ColorImage in;
  in.create(xo, yo, mv);

  while (input.FrameNumber() < first &&
         input.read())
    {
      if (verbose)
        {
          if (input.FrameNumber() % 100 == 0)
            cout << "skipping frame: " << input.FrameNumber() << endl;
        }
    }

  int frames = 0;
  if (last < 0)
    last = numeric_limits<int>::max();
  while (input.FrameNumber() < last &&
         input.read(in))
    {
      frames++;
      Image g;
      g.create(xo, yo, mv);

      WindowWalker ww(g);
      for (ww.init(); !ww.ready(); ww.next())
        {
          int gv = 0;
          ColorValue cv = in.getPixelUnchecked(ww);
          switch (colorMode)
            {
            case 'r':
              gv = cv.red;
              break;
            case 'g':
              gv = cv.green;
              break;
            case 'b':
              gv = cv.blue;
              break;
            case 'i':
              gv = cv.getGray();
              break;
            }
          g.setPixelUnchecked(ww, gv);
        }

      ivector.push_back(g);
      in.create(xo, yo, mv); // recreate input image
      if (verbose)
        if (frames % 100 == 0)
          {
            cout << "reading frame #" << input.FrameNumber();
            cout << " (" << frames << " frames)" << endl;;
          }
    }
}

void readImages(const string& fn,
                vector<ColorImage>& ivector, int& fps,
                int first, int last)
{
  int xo, yo, mv;

  VideoFile input(fn);
  input.getPara(xo, yo, mv, fps);

  if (verbose)
    {
      cout << "video " << fn << " with " << xo << "x" << yo ;
      cout << ", " << fps << " frames per second" << endl;
    }

  ColorImage in;
  in.create(xo, yo, mv);

  while (input.FrameNumber() < first &&
         input.read())
    {
      if (verbose)
        {
          if (input.FrameNumber() % 100 == 0)
            cout << "skipping frame: " << input.FrameNumber() << endl;
        }
    }

  int frames = 0;
  if (last < 0)
    last = numeric_limits<int>::max();
  while (input.FrameNumber() < last &&
         input.read(in))
    {
      frames++;
      ivector.push_back(in);

      in.create(xo, yo, mv); // recreate input image

      if (verbose)
        if (frames % 100 == 0)
          {
            cout << "reading frame #" << input.FrameNumber();
            cout << " (" << frames << " frames)" << endl;;
          }
    }
}

void writePlotFile(const string& name, const vector<double>& v)
{
  ofstream os(name);
  if (verbose)
    cout << "writing " << name << endl;
  for (int i = 0; i < v.size(); i++)
    os << i << " " << v[i] << endl;
}

void writePlotFile(const string& name, const vector<double>& v1, const vector<double>& v2)
{
  ofstream os(name);
  if (verbose)
    cout << "writing " << name << endl;
  for (int i = 0; i < v1.size(); i++)
    os << i << " " << v1[i] << " " << v2[i] << endl;
}
