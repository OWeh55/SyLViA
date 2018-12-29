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
