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

void scanVideo(const string& fn,
               vector<double>& gsum,
               vector<double>& dgsum,
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

  Image thisImage;
  thisImage.create(xSize, ySize, mv);
  Image lastImage;
  lastImage.create(xSize, ySize, mv);

  gsum.clear();
  dgsum.clear();

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

      //      cout << frames << " frames read" << endl;
      //      cout << "FrameNumber: " << input.FrameNumber() << endl;

      WindowWalker ww(thisImage);
      IPoint shift = window.p1;
      switch (colorMode)
        {
        case 'r':
          for (ww.init(); !ww.ready(); ww.next())
            {
              ColorValue cv = in.getPixelUnchecked(ww + shift);
              thisImage.setPixelUnchecked(ww, mv - cv.red);
            }
          break;
        case 'g':
          for (ww.init(); !ww.ready(); ww.next())
            {
              ColorValue cv = in.getPixelUnchecked(ww + shift);
              thisImage.setPixelUnchecked(ww, mv - cv.green);
            }
          break;
        case 'b':
          for (ww.init(); !ww.ready(); ww.next())
            {
              ColorValue cv = in.getPixelUnchecked(ww + shift);
              thisImage.setPixelUnchecked(ww, mv - cv.blue);
            }
          break;
        case 'i':
          for (ww.init(); !ww.ready(); ww.next())
            {
              ColorValue cv = in.getPixelUnchecked(ww + shift);
              thisImage.setPixelUnchecked(ww, cv.getGray());
            }
          break;
        }
      if (frames == 1) // first frame read
        CopyImg(thisImage, lastImage);

      // calculate gray value sum
      // and absolute difference to previous image
      double gSum = graySum(thisImage);
      double dgSum = absGrayDiff(thisImage, lastImage);
      gsum.push_back(gSum);
      dgsum.push_back(dgSum);
      CopyImg(thisImage, lastImage);
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

void readImage(VideoFile& v, int frameNr, ColorImage& img)
{
  while (v.FrameNumber() < frameNr)
    v.read();
  v.read(img);
}

void readSequence(VideoFile& v,
                  double leftBoundary, double rightBoundary,
                  int colorMode,
                  vector<ImageD>& seq,
                  ColorImage& cImg)
{
  int xs, ys, mv, fps;
  v.getPara(xs, ys, mv, fps);

  double patternLength = (rightBoundary - leftBoundary) / sequenceLength;
  leftBoundary += patternLength; // skip white pattern
  seq.resize(sequenceDescription.size());

  ImageD r, g, b;
  r.create(xs, ys);
  r.set(0);
  g.create(xs, ys);
  g.set(0);
  b.create(xs, ys);
  b.set(0);
  int nPattern = sequenceDescription.size();

  for (int i = 0; i < nPattern; i++) // all pattern pairs (pos+neg)
    {
      int posIdx = RoundInt(leftBoundary + i * (2 * patternLength) + patternLength / 2);
      int negIdx = RoundInt(leftBoundary + i * (2 * patternLength) + patternLength + patternLength / 2);
      ColorImage pos;
      pos.create(xs, ys, mv);
      readImage(v, posIdx, pos);
      ColorImage neg;
      neg.create(xs, ys, mv);
      readImage(v, negIdx, neg);
      seq[i].create(xs, ys);
      WindowWalker w(seq[i]);
      for (w.init(); !w.ready(); w.next())
        {
          ColorValue pColor = pos.getPixel(w);
          int pval = pColor.getGray();
          ColorValue nColor = neg.getPixel(w);
          int nval = nColor.getGray();
          seq[i].setPixel(w, pval - nval);
          r.setPixel(w, r.getPixel(w) + pColor.red + nColor.red);
          g.setPixel(w, g.getPixel(w) + pColor.green + nColor.green);
          b.setPixel(w, b.getPixel(w) + pColor.blue + nColor.blue);
        }
    }
  cImg.create(xs, ys, mv);
  WindowWalker w(cImg);
  for (w.init(); !w.ready(); w.next())
    {
      cImg.setPixel(w, ColorValue(r.getPixel(w) / nPattern / 2,
                                  g.getPixel(w) / nPattern / 2,
                                  b.getPixel(w) / nPattern) / 2);
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

void writePlotFile(const string& name,
                   const vector<double>& v1,
                   const vector<double>& v2)
{
  ofstream os(name);
  if (verbose)
    cout << "writing " << name << endl;
  for (int i = 0; i < v1.size(); i++)
    os << i << " " << v1[i] << " " << v2[i] << endl;
}
