#include "v23d.h"

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

void analyse(const std::vector<ice::Image>& cv, vector<double>& sb,
             int period /* =0 */)
{
  vector<double> g(nFrames);
  for (int i = 0; i < nFrames; i++)
    g[i] = graySum(cv[i]);
  if (debug & 1)
    writePlotFile("g.gp", g);

  vector<double> dg(nFrames - 1);
  for (int i = 0; i < nFrames - 1; i++)
    dg[i] = absGrayDiff(cv[i], cv[i + 1]);
  if (debug & 1)
    writePlotFile("dg.gp", dg);

  vector<double> gps = powerSpectrum(g);
  if (debug & 1)
    writePlotFile("gps.gp", gps);
  //cout << "a priori max freq " <<  nFrames / sequenceLength / 2 << endl;
  double gfMax = findMaxBetween(gps, 2, nFrames / sequenceLength / 2);
  if (verbose)
    {
      cout << "Frequence of sequence: " << gfMax << endl;
      cout << "Estimated sequence length: " << nFrames / gfMax << endl;
    }

  vector<double> dgps = powerSpectrum(dg);
  if (debug & 1)
    writePlotFile("dgps.gp", dgps);
  //  cout << "estimation: " <<  sequenceLength* gfMax << endl;
  //  cout << "+/- : " << sequenceLength* gfMax / 2 << endl;
  double pfMax = findMaxAround(dgps, sequenceLength * gfMax,
                               sequenceLength * gfMax / 2);
  if (verbose)
    {
      cout << "Frequence of pattern: " << pfMax << endl;
      cout << "Estimated pattern length: " << nFrames / pfMax << endl;
      cout << "Corrected sequence length: " << nFrames / pfMax* sequenceLength << endl;
    }

  if (period > 0)
    {
      if (verbose)
        cout << "using given estimation of period: " << period << endl;
    }
  else
    // use estimation from spectra
    period = nFrames / pfMax * sequenceLength;

  vector<double> ac = autoCorrelationFromPowerSpectrum(gps);
  if (debug & 1)
    writePlotFile("gac.gp", ac);

  double sLen = findMaxBetween(ac, 8 * period / 10, 12 * period / 10);
  if (verbose)
    cout << "Cycle len from autocorrelation: " << sLen << endl;

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
    writePlotFile("ave.gp", average);

  double x0 = findMaxBetween(dAverage);
  if (verbose)
    cout << "coarse boundaries: " << x0 << "  + k * " << sLen << endl;

  vector<double> gc = dt(g); // \delta g_i
  if (debug & 1)
    writePlotFile("delta.gp", gc);

  sb.clear();
  double x = x0;
  while (x < nFrames)
    {
      // cout << "x: " << x << " +/- " << sLen*2/sequenceLength << endl;
      double x_better = findMaxAround(gc, x, sLen * 2 / sequenceLength);
      sb.push_back(x_better - 0.5); // boundary is between this and previous image
      x = x_better;
      if (debug & 2)
        {
          int x1 = x - 1;
          if (x1 < 0)
            x1 = 0;
          int x2 = x + 1;
          if (x2 > nFrames - 1)
            x2 = nFrames - 1;
          for (int i = x1; i <= x2; i++)
            Show(ON, cv[i], "Image " + to_string(i));
          GetChar();
          for (int i = x1; i <= x2; i++)
            Show(OFF, cv[i]);
        }
      x = x_better + sLen;
    }
  if (verbose)
    cout << "boundaries: " << sb << endl;
}

double absGrayDiff(const ColorImage& img1,
                   const ColorImage& img2)
{
  int n = 0;
  int gsum = 0;
  WindowWalker ww(img1);
  for (ww.init(); !ww.ready(); ww.next())
    {
      ColorValue c1 = img1.getPixelUnchecked(ww);
      ColorValue c2 = img2.getPixelUnchecked(ww);

      int gray1 =  c1.getGray();
      int gray2 =  c2.getGray();
      int grayval = gray1 - gray2;
      gsum += abs(grayval);
      n++;
    }
  return double(gsum) / n;
}

double graySum(const ColorImage& img)
{
  int gsum = 0;
  int n = 0;
  WindowWalker ww(img);
  for (ww.init(); !ww.ready(); ww.next())
    {
      ColorValue c = img.getPixelUnchecked(ww);
      int gray =  c.getGray();
      gsum += gray;
      n++;
    }
  return double(gsum) / n;
}

void analyse(const std::vector<ice::ColorImage>& cv, vector<double>& sb,
             int period /* =0 */)
{
  vector<double> g(nFrames);
  for (int i = 0; i < nFrames; i++)
    g[i] = graySum(cv[i]);
  if (debug & 1)
    writePlotFile("g.gp", g);

  vector<double> dg(nFrames - 1);
  for (int i = 0; i < nFrames - 1; i++)
    dg[i] = absGrayDiff(cv[i], cv[i + 1]);
  if (debug & 1)
    writePlotFile("dg.gp", dg);

  vector<double> gps = powerSpectrum(g);
  if (debug & 1)
    writePlotFile("gps.gp", gps);
  //cout << "a priori max freq " <<  nFrames / sequenceLength / 2 << endl;
  double gfMax = findMaxBetween(gps, 2, nFrames / sequenceLength / 2);
  if (verbose)
    {
      cout << "Frequence of sequence: " << gfMax << endl;
      cout << "Estimated sequence length: " << nFrames / gfMax << endl;
    }

  vector<double> dgps = powerSpectrum(dg);
  if (debug & 1)
    writePlotFile("dgps.gp", dgps);
  //  cout << "estimation: " <<  sequenceLength* gfMax << endl;
  //  cout << "+/- : " << sequenceLength* gfMax / 2 << endl;
  double pfMax = findMaxAround(dgps, sequenceLength * gfMax,
                               sequenceLength * gfMax / 2);
  if (verbose)
    {
      cout << "Frequence of pattern: " << pfMax << endl;
      cout << "Estimated pattern length: " << nFrames / pfMax << endl;
      cout << "Corrected sequence length: " << nFrames / pfMax* sequenceLength << endl;
    }

  if (period > 0)
    {
      if (verbose)
        cout << "using given estimation of period: " << period << endl;
    }
  else
    // use estimation from spectra
    period = nFrames / pfMax * sequenceLength;

  vector<double> ac = autoCorrelationFromPowerSpectrum(gps);
  if (debug & 1)
    writePlotFile("gac.gp", ac);

  double sLen = findMaxBetween(ac, 8 * period / 10, 12 * period / 10);
  if (verbose)
    cout << "Cycle len from autocorrelation: " << sLen << endl;

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
    writePlotFile("ave.gp", average);

  double x0 = findMaxBetween(dAverage);
  if (verbose)
    cout << "coarse boundaries: " << x0 << "  + k * " << sLen << endl;

  vector<double> gc = dt(g); // \delta g_i
  if (debug & 1)
    writePlotFile("delta.gp", gc);

  sb.clear();
  double x = x0;
  while (x < nFrames)
    {
      // cout << "x: " << x << " +/- " << sLen*2/sequenceLength << endl;
      double x_better = findMaxAround(gc, x, sLen * 2 / sequenceLength);
      sb.push_back(x_better - 0.5); // boundary is between this and previous image
      x = x_better;
      if (debug & 2)
        {
          int x1 = x - 1;
          if (x1 < 0)
            x1 = 0;
          int x2 = x + 1;
          if (x2 > nFrames - 1)
            x2 = nFrames - 1;
          for (int i = x1; i <= x2; i++)
            Show(ON, cv[i], "Image " + to_string(i));
          GetChar();
          for (int i = x1; i <= x2; i++)
            Show(OFF, cv[i]);
        }
      x = x_better + sLen;
    }
  if (verbose)
    cout << "boundaries: " << sb << endl;
}
