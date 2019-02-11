#include <fstream>
#include <iostream>
#include <iomanip>

#include "FileName.h"
#include "v23d.h"
#include "v2file.h"

using namespace std;

vector<string> split(const string& s, char del)
{
  vector<string> result(1);
  for (int i = 0; i < s.size(); i++)
    {
      char c = s[i];
      if (c != del)
        result.back() += c;
      else
        result.push_back("");
    }
  return result;
}

void fileNames(const string& baseFileName, string& textureFN, string& phaseFN, string& calFN)
{
  FileName fnt(baseFileName);
  fnt.setExtension("pnm");
  FileName fnp(baseFileName);
  fnp.setExtension("pgm");
  FileName fnc(baseFileName);
  fnc.setExtension("cal");

  string basename = fnt.getName();
  vector<string> parts = split(basename, '_');

  // remove content marker
  if (parts.back() == "phase" || parts.back() == "texture" || parts.back() == "cal" || parts.back().empty())
    parts.pop_back();
  // recombine parts
  basename = "";
  for (const auto& s : parts)
    {
      if (basename.empty())
        basename = s;
      else
        basename += "_" + s;
    }

  fnt.setName(basename + "_texture");
  fnp.setName(basename + "_phase");
  fnc.setName(basename + "_cal");
  textureFN = fnt;
  phaseFN = fnp;
  calFN = fnc;
}

void readImage(VideoFile& v, int frameNr, ColorImage& img)
{
  if (frameNr < v.FrameNumber())
    throw IceException("readImage", "frame index is falling");
  while (v.FrameNumber() < frameNr)
    v.read();
  v.read(img);
}

void readSequence(VideoFile& v,
                  double leftBoundary, double rightBoundary,
                  int sequenceLength,
                  int colorMode,
                  const Window& readWindow,
                  vector<ImageD>& seq,
                  ColorImage& cImg)
{
  int xo, yo, mv, fps;
  v.getPara(xo, yo, mv, fps);

  Window window(readWindow);
  if (window.p2.x >= xo)
    window.p2.x = xo - 1;

  if (window.p2.y >= yo)
    window.p2.y = yo - 1;

  int xs = window.Width();
  int ys = window.Height();

  double patternLength = (rightBoundary - leftBoundary) / sequenceLength;
  leftBoundary += patternLength; // skip white pattern
  seq.resize(sequenceLength - 2);

  ImageD r, g, b;
  r.create(xs, ys);
  r.set(0);
  g.create(xs, ys);
  g.set(0);
  b.create(xs, ys);
  b.set(0);
  int nPattern = sequenceLength - 2;

  for (int i = 0; i < nPattern; i++) // all pattern pairs (pos+neg)
    {
      int posIdx = RoundInt(leftBoundary + i * (2 * patternLength) + patternLength / 2);
      int negIdx = RoundInt(leftBoundary + i * (2 * patternLength) + patternLength + patternLength / 2);
      ColorImage pos;
      pos.create(xo, yo, mv);
      readImage(v, posIdx, pos);
      ColorImage neg;
      neg.create(xo, yo, mv);
      readImage(v, negIdx, neg);
      seq[i].create(xs, ys);
      WindowWalker w(seq[i]);
      for (w.init(); !w.ready(); w.next())
        {
          IPoint original = w + window.p1;
          ColorValue pColor = pos.getPixel(original);
          int pval = pColor.getGray();
          ColorValue nColor = neg.getPixel(original);
          int nval = nColor.getGray();
          seq[i].setPixel(w, nval - pval);
          r.setPixel(w, r.getPixel(w) + pColor.red + nColor.red);
          g.setPixel(w, g.getPixel(w) + pColor.green + nColor.green);
          b.setPixel(w, b.getPixel(w) + pColor.blue + nColor.blue);
        }
      seq[i].adaptLimits();
      /*
      Show(GRAY,seq[i],"pattern " + to_string(i));
      GetChar();
      Show(OFF,seq[i]);
      */
    }

  cImg.create(xs, ys, mv);
  WindowWalker w(cImg);
  for (w.init(); !w.ready(); w.next())
    {
      cImg.setPixel(w, ColorValue(r.getPixel(w),
                                  g.getPixel(w),
                                  b.getPixel(w)) / nPattern / 2);
    }
}

void writeCalib(const string& name, const vector<double>& para)
{
  ofstream os(name);
  if (verbose)
    cout << "writing " << name << endl;
  os << "cal1" << endl;
  for (int i = 0; i < para.size(); i++)
    os << setw(8) << para[i] << endl;
}

bool readCalib(const string& name, vector<double>& para)
{
  ifstream is(name);
  if (!is.good())
    return false;
  string tag;
  getline(is, tag);
  if (tag == "cal1")
    {
      para.clear();
      for (int i = 0; i < 18; i++)
        {
          double p;
          is >> p;
          para.push_back(p);
        }
    }
  else
    throw IceException("readCalib", "unkbown format");
  return true;
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
