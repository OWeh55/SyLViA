#include <fstream>
#include <iostream>

#include <vector>

#include <image.h>

#include "v23d.h"

using namespace std;

double findMaxBetween(const vector<double>& dg, int i1 /* =0 */, int i2 /* = -1 */)
// we return double here.
// perhaps we will find a better estimation later, which interpolates
// indices.
{
  if (i2 == -1)
    i2 = dg.size() - 1;
  if (i1 < 0 || i2 < i1 || i2 >= dg.size())
    throw IceException("findMaxBetween", "Index out of range");
  double max = dg[i2];
  int idx = i2;
  for (int i = i1; i < i2; i++)
    {
      if (dg[i] > max)
        {
          max = dg[i];
          idx = i;
        }
    }
  return idx;
}

double findMaxAround(const vector<double>& dg, int is, int di)
// we return double here.
// perhaps we will find a better estimation later, which interpolates
// indices.
{
  int i1 = is - di;
  if (i1 < 0)
    i1 = 0;
  int i2 = is + di;
  if (i2 >= dg.size())
    i2 = dg.size() - 1;
  return findMaxBetween(dg, i1, i2);
}

// cyclic differences between elements
vector<double> dt(const vector<double>& v)
{
  vector<double> result(v.size());
  double last = v.back();
  for (int i = 0; i < v.size(); i++)
    {
      result[i] = v[i] - last;
      last = v[i];
    }
  return result;
}
