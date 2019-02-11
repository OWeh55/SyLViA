#include "v23d.h"
#include "v2file.h"


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
