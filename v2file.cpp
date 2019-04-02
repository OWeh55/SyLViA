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

void fileNames(const string& baseFileName,
               string& textureFN,
               string& phaseFN,
               string& calFN)
{
  FileName fnt(baseFileName);
  cout << (string)fnt << endl;
  fnt.setExtension("pnm");
  fnt.setSpec("texture");
  cout << (string)fnt << endl;
  FileName fnp(baseFileName);
  fnp.setExtension("pgm");
  fnp.setSpec("phase");
  FileName fnc(baseFileName);
  fnc.setExtension("cal");
  fnc.setSpec("cal");
  textureFN = fnt;
  phaseFN = fnp;
  calFN = fnc;
  cout << textureFN << " " << phaseFN << " " << calFN << endl;
}

void readImage(VideoFile& v, int frameNr, ColorImage& img)
{
  if (frameNr < v.FrameNumber())
    throw IceException("readImage", "frame index is falling");
  while (v.FrameNumber() < frameNr)
    v.read();
  v.read(img);
}

void readNImages(VideoFile& v, int frameNr, ColorImage& img, int n)
{
  int n2 = n / 2;
  ColorImage vs;
  vs.create(img.xsize, img.ysize, img.maxval * n);
  vs.set(ColorValue(0, 0, 0));
  ColorImage vt;
  vt.create(img);
  // cout << "frameNr: " << frameNr << endl;
  for (int idx = frameNr - n2; idx <= frameNr + n2; idx++)
    {
      readImage(v, idx, vt);
      WindowWalker w(vs);
      for (w.init(); !w.ready(); w.next())
        {
          vs.setPixel(w, vs.getPixel(w) + vt.getPixel(w));
        }
    }

  WindowWalker w(vs);
  double f = 1.0 / n;
  for (w.init(); !w.ready(); w.next())
    {
      img.setPixel(w, vs.getPixel(w) * f);
    }
}

void readSequence(VideoFile& v,
                  double leftBoundary, double rightBoundary,
                  int sequenceLength,
                  int colorMode,
                  const Window& readWindow,
                  int nFrames,
                  vector<ImageD>& seq,
                  ColorImage& cImg,
                  ColorImage& deviation)
{
  int xo, yo, mv, fps;
  v.getPara(xo, yo, mv, fps);

  // adapt window size to size of frames of video
  Window window(readWindow);
  if (window.p2.x >= xo)
    window.p2.x = xo - 1;

  if (window.p2.y >= yo)
    window.p2.y = yo - 1;

  int xs = window.Width();
  int ys = window.Height();

  double patternLength = (rightBoundary - leftBoundary) / sequenceLength;
  if (nFrames >= patternLength)
    throw IceException("readSequence", "number of frames to average exeeds patternLength");

  leftBoundary += patternLength; // skip white pattern

  int nPattern = (sequenceLength - 2) / 2;
  seq.resize(nPattern);

  vector<ColorImage> rgb(nPattern);

  for (int i = 0; i < nPattern; i++) // all pattern pairs (pos + neg)
    {
      int posIdx = RoundInt(leftBoundary + i * (2 * patternLength) + patternLength / 2);
      int negIdx = RoundInt(leftBoundary + i * (2 * patternLength) + patternLength + patternLength / 2);
      // cout << posIdx << " " << negIdx << endl;
      ColorImage pos;
      pos.create(xo, yo, mv);
      readNImages(v, posIdx, pos, nFrames);
      ColorImage neg;
      neg.create(xo, yo, mv);
      readNImages(v, negIdx, neg, nFrames);
      seq[i].create(xs, ys);
      rgb[i].create(xs, ys, mv * 2);
      WindowWalker w(seq[i]);
      for (w.init(); !w.ready(); w.next())
        {
          IPoint original = w + window.p1;
          ColorValue pColor = pos.getPixel(original);
          int pval = pColor.getGray();
          ColorValue nColor = neg.getPixel(original);
          int nval = nColor.getGray();
          seq[i].setPixel(w, nval - pval);
          rgb[i].setPixel(w, pColor + nColor);
        }
      seq[i].adaptLimits();
      // cout << i << ": " << seq[i].minValue() << " .. " << seq[i].maxValue() << endl;
      /*
      Show(GRAY,seq[i],"pattern " + to_string(i));
      GetChar();
      Show(OFF,seq[i]);
      */
    }

  cImg.create(xs, ys, mv);
  deviation.create(xs, ys, mv);
  WindowWalker w(cImg);
  for (w.init(); !w.ready(); w.next())
    {
      ColorValue cv(0, 0, 0);
      for (int i = 0; i < nPattern; i++)
        cv += rgb[i].getPixel(w);
      cv /= nPattern; // mean value
      cImg.setPixel(w, cv / 2);
      ColorValue dev(0, 0, 0);
      for (int i = 0; i < nPattern; i++)
        {
          ColorValue d = cv - rgb[i].getPixel(w);
          dev.red += abs(d.red);
          dev.green += abs(d.green);
          dev.blue += abs(d.blue);
        }
      dev /= nPattern * 2;
      deviation.setPixel(w, dev);
    }
}

void writeCalib(const string& name, const vector<double>& para, char model)
{
  ofstream os(name);
  if (verbose)
    cout << "writing " << name << endl;
  // we start to use second format with different models
  os << "cal2" << endl;
  os << model << endl;
  for (int i = 0; i < para.size(); i++)
    os << setw(8) << para[i] << endl;
}

bool readCalib(const string& name, vector<double>& para, char& model)
{
  ifstream is(name);
  if (!is.good())
    return false;
  string tag;
  getline(is, tag);
  if (tag == "cal1")
    {
      model = 'p';
      para.clear();
      for (int i = 0; i < 18; i++)
        {
          double p;
          is >> p;
          para.push_back(p);
        }
    }
  else if (tag == "cal2")
    {
      string line;
      getline(is, line);
      model = line[0];
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
