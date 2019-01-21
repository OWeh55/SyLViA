#include <fstream>
#include <iostream>

#include "v23d.h"

using namespace std;

void readImages(const string& fn,
                vector<Image>& ivector,
                int& fps,
                int first, int last,
                int colorMode,
                const Window& readWindow)
{
  int xo, yo, mv;

  VideoFile input(fn);
  input.getPara(xo, yo, mv, fps);

  if (verbose)
    {
      cout << "video " << fn << " with " << xo << "x" << yo ;
      cout << ", " << fps << " frames per second" << endl;
    }
  Window window(readWindow);

  if (window.p2.x >= xo)
    window.p2.x = xo - 1;

  if (window.p2.y >= yo)
    window.p2.y = yo - 1;

  int xSize = window.Width();
  int ySize = window.Height();

  ColorImage in;
  in.create(xo, yo, mv);
  if (debug & 16)
    Show(ON, in);

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
      g.create(xSize, ySize, mv);

      WindowWalker ww(g);
      IPoint shift = window.p1;
      switch (colorMode)
        {
        case 'r':
          for (ww.init(); !ww.ready(); ww.next())
            {
              ColorValue cv = in.getPixelUnchecked(ww + shift);
              g.setPixelUnchecked(ww, mv - cv.red);
            }
          break;
        case 'g':
          for (ww.init(); !ww.ready(); ww.next())
            {
              ColorValue cv = in.getPixelUnchecked(ww + shift);
              g.setPixelUnchecked(ww, mv - cv.green);
            }
          break;
        case 'b':
          for (ww.init(); !ww.ready(); ww.next())
            {
              ColorValue cv = in.getPixelUnchecked(ww + shift);
              g.setPixelUnchecked(ww, mv - cv.blue);
            }
          break;
        case 'i':
          for (ww.init(); !ww.ready(); ww.next())
            {
              ColorValue cv = in.getPixelUnchecked(ww + shift);
              g.setPixelUnchecked(ww, cv.getGray());
            }
          break;
        }
      ivector.push_back(g);
      // in.create(xo, yo, mv); // recreate input image
      if (verbose)
        if (frames % 100 == 0)
          {
            cout << "reading frame #" << input.FrameNumber();
            cout << " (" << frames << " frames)" << endl;;
          }
    }
  if (debug & 16)
    Show(OFF, in);
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
