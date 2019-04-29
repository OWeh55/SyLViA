// tests for surface smoothing with structure tensor

#include <iostream>
#include <image.h>

matrix<double> genST(const vector<Vector3d>& points, const Vector3d& center)
{
  matrix<double> res(3, 3);
  res.set(0);
  for (auto p : points)
    {
      Vector3d delta = p - center;
      res[0][0] += delta.x * delta.x;
      res[1][0] += delta.y * delta.x;
      res[2][0] += delta.z * delta.x;
      res[1][1] += delta.y * delta.y;
      res[2][1] += delta.z * delta.y;
      res[2][2] += delta.z * delta.z;
    }
  res[0][1] = res[1][0];
  res[0][2] = res[2][0];
  res[1][2] = res[2][1];
  return res;
}

matrix<double> genST(const vector<Vector3d>& points,
                     const Vector3d& center, double dist)
{
  vector<Vector3d> selected;
  for (auto p : points)
    {
      if ((p - center).length() <= dist)
        selected.push_back(p);
    }
  cout << selected.size() << endl;
  return genST(selected, center);
}

double symRand(double mVal)
{
  return (drand48() - 0.5) * mVal * 2;
}

void applyNoise(vector<Vector3d>& points, double nVal)
{
  for (auto& p : points)
    {
      p += Vector3d(symRand(nVal), symRand(nVal), symRand(nVal));
    }
}

int main(int argc, char** argv)
{
  vector<Vector3d> points;
  matrix<double> st(3, 3);

  for (int x = 0; x < 10; x++)
    for (int y = 0; y < 10; y++)
      points.push_back(Vector3d(x, y, -0.1 * x - 0.1 * y + 10));

  applyNoise(points, 0.1);

  Vector3d center = points[55];
  st = genST(points, center, 2.3);
  cout << st << endl;

  vector<double> eval;
  vector<vector<double>> evect;
  Eigenvalue(st, eval, evect);
  cout << eval << endl;
  cout << evect << endl;

  return 0;
}
