#include <fstream>
#include <iostream>

#include <vector>

#include <image.h>

#include "v23d.h"

using namespace std;

vector<double> powerSpectrum(const vector<double>& g)
{
  FourierTrafo ft(g.size());
  ft.setInput(g);

  vector<double> re(g.size());
  vector<double> im(g.size());
  ft.getResult(re, im);

  vector<double> ps(g.size() / 2);

  // to power spectrum
  for (int i = 0; i < ps.size(); i++)
    ps[i] = re[i] * re[i] + im[i] * im[i];
  // ignore dc
  ps[0] = 0.0;
  return ps;
}

vector<double> autoCorrelation(const vector<double>& g)
{
  FourierTrafo ft(g.size());
  ft.setInput(g);

  vector<double> re(g.size());
  vector<double> im(g.size());
  ft.getResult(re, im);

  // to power spectrum
  for (int i = 0; i < g.size(); i++)
    re[i] = re[i] * re[i] + im[i] * im[i];

  re[0] = 0.0;

  ft.setInput(re);
  ft.getResult(re);
  return re;
}

vector<double> autoCorrelationFromPowerSpectrum(const vector<double>& ps)
{
  vector<double> eps(ps.size() * 2);
  for (int i = 0; i < ps.size(); i++)
    eps[i] = ps[i];
  for (int i = 0; i < ps.size(); i++)
    eps[eps.size() - 1 - i] = ps[i];
  FourierTrafo ft(eps.size());
  ft.setInput(eps);
  vector<double> re;
  ft.getResult(re);
  return re;
}

void zeroPadding(vector<double>& v, int before, int after)
{
  // we add the additional samples at the end
  // since we use the cyclic model, it is not important, that padding
  // happens at both ends
  // we use the mean value as "zero"
  int oldSize = v.size();
  double sum = 0;
  for (double d : v)
    sum += d;
  vector<double> result(before + oldSize + after, sum / oldSize);
  for (int i = 0; i < oldSize; i++)
    result[i + before] = v[i];
  v = result;
}
