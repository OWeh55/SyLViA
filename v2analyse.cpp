#include "v23d.h"
#include "v2file.h"

double absGrayDiff(const Image& img1,
                   const Image& img2)
{
  int n = 0;
  int gsum = 0;
  WindowWalker ww(img1);
  for (ww.init(); !ww.ready(); ww.next())
    {
      int gray1 = img1.getPixelUnchecked(ww);
      int gray2 = img2.getPixelUnchecked(ww);
      int grayval = gray1 - gray2;
      gsum += abs(grayval);
      n++;
    }
  return double(gsum) / n;
}

double graySum(const Image& img)
{
  int gsum = 0;
  int n = 0;
  WindowWalker ww(img);
  for (ww.init(); !ww.ready(); ww.next())
    {
      int gray = img.getPixelUnchecked(ww);
      gsum += gray;
      n++;
    }
  return double(gsum) / n;
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


void analysis(const std::vector<double>& gp,
              const std::vector<double>& dgp,
              int fps,
              vector<double>& sb,
              int& cycleLength,
              int& cycleStart)
{
  vector<double> g(gp);
  vector<double> dg(dgp);

  int nFrames = g.size();
  if (dg.size() != nFrames)
    throw IceException("analysis", "vectors have different sizes");

  zeroPadding(g, 0, nFrames);
  zeroPadding(dg, 0, nFrames);

  vector<double> gps = powerSpectrum(g);
  if (debug & 1)
    writePlotFile("gvps.gp", gps);

  if (sequenceLength <= 0)
    throw IceException("analysis", "sequenceLength invalid");

  if (verbose)
    cout << "a priori max freq " <<  nFrames / sequenceLength / 2 << endl;

  // we apply a "high pass" to suppress slow changes
  // 0.1 Hz <-> 10 s <-> fps*10 Frames
  int hp = (double)nFrames / fps / 10;
  if (hp < 2)
    hp = 2;
  //  cout << "Hochpass: " << hp << " (" << hp * 2 << ")" << endl;
  double gfMax = findMaxBetween(gps, 2 * hp, 2 * nFrames / sequenceLength / 2) / 2;
  if (verbose)
    {
      cout << "Sequences: " << gfMax << endl;
      cout << "Estimated sequence length: " << nFrames / gfMax << endl;
    }

  vector<double> dgps = powerSpectrum(dg);
  if (debug & 1)
    writePlotFile("gvadps.gp", dgps);
  cout << "estimation: " <<  sequenceLength* gfMax << endl;
  //  cout << "+/- : " << sequenceLength* gfMax / 2 << endl;
  double pfMax = findMaxAround(dgps, 2 * sequenceLength * gfMax,
                               sequenceLength * gfMax / 2) / 2;
  if (verbose)
    {
      cout << "Frequence of pattern: " << pfMax << endl;
      cout << "Estimated pattern length: " << nFrames / pfMax << endl;
      cout << "Corrected sequence length: " << nFrames / pfMax* sequenceLength << endl;
    }

  if (cycleLength > 0)
    {
      if (verbose)
        cout << "using given cycle length: " << cycleLength << endl;
    }
  else
    // use estimation from spectrum
    cycleLength = nFrames / pfMax * sequenceLength;

  //  vector<double> ac = autoCorrelationFromPowerSpectrum(gps);
  vector<double> ac = autoCorrelation(g);
  if (debug & 1)
    writePlotFile("gvac.gp", ac);

  double sLen = findMaxBetween(ac, 8 * cycleLength / 10, 12 * cycleLength / 10);
  if (verbose)
    cout << "Cycle length (from autocorrelation): " << sLen << endl;

  int iLen = sLen;
  // split video in parts of len sLen and average values
  vector<double> average(iLen, 0.0);
  for (int i = 0; i + iLen <= g.size(); i += iLen) // all parts fully inside
    {
      for (int k = 0; k < iLen; k++)
        average[k] += g[k + i];
    }

  vector<double> dAverage = dt(average);

  if (debug & 1)
    writePlotFile("averagegv.gp", average);

  if (cycleStart <= 0) // no start frame given
    {
      // find black to white change in averaged cycle
      cycleStart = findMaxBetween(dAverage);
    }

  if (verbose)
    cout << "coarse boundaries: " << cycleStart << "  + k * " << sLen << endl;

  vector<double> gc = dt(g); // \delta g_i
  if (debug & 1)
    writePlotFile("gvdt.gp", gc);

  sb.clear();
  double x = cycleStart;
  while (x < nFrames)
    {
      // cout << "x: " << x << " +/- " << sLen*2/sequenceLength << endl;
      double x_better = findMaxAround(gc, x, sLen * 2 / sequenceLength);
      sb.push_back(x_better - 0.5); // boundary is between this and previous image
      x = x_better + sLen;
    }
  if (verbose)
    cout << "refined boundaries: " << sb << endl;
}
