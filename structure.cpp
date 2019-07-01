#include <iostream>
#include <image.h>

#include "to3d.h"

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
  return genST(selected, center);
}

vector<Vector3d> getSurroundingPoints(const KDTree<Vector3d>& points,
                                      const Vector3d& center,
                                      double dist)
{
  vector<const Vector3d*> pp;
  points.findNeighbors(center, dist, pp);
  vector<Vector3d> result;
  for (auto p : pp)
    result.push_back(*p);
  return result;
}

vector<Vector3d> getSurroundingPoints(const SWorld& points,
                                      const IPoint& center, double dist)
{
  vector<Vector3d> selected;

  int xSize = points.rows();
  int ySize = points.cols();

  Vector3d centerVector = points[center.x][center.y].position;

  for (int y = 0; y < ySize; y++)
    for (int x = 0; x < xSize; x++)
      {
        const SPixel& here(points[x][y]);
        if (here.valid)
          {
            if ((here.position - centerVector).length() < dist)
              selected.push_back(here.position);
          }
      }
  return selected;
}

void makeTree(const SWorld& world, KDTree<Vector3d>& tree)
{
  int xSize = world.rows();
  int ySize = world.cols();
  vector<Vector3d> points;
  for (int y = 0; y < ySize; y++)
    for (int x = 0; x < xSize; x++)
      {
        const SPixel& here(world[x][y]);
        if (here.valid)
          {
            points.push_back(here.position);
          }
      }
  tree.create(points);
}

void smooth(SWorld& world, int dist)
{
  int xSize = world.rows();
  int ySize = world.cols();

  matrix<Vector3d> newXyz(xSize, ySize);
  for (int y = 0; y < ySize; y++)
    for (int x = 0; x < xSize; x++)
      newXyz[x][y] = world[x][y].position;


  KDTree<Vector3d> tree;
  makeTree(world, tree);

  for (int y = 0; y < ySize; y++)
    {
      if (verbose)
        {
          cout << ".";
          cout.flush();
        }
      for (int x = 0; x < xSize; x++)
        {
          //    cout << ".";cout.flush();
          const SPixel& h = world[x][y];
          if (h.valid)
            {
              // vector<Vector3d> sPoints = getSurroundingPoints(world, IPoint(x, y), dist);
              vector<Vector3d> sPoints = getSurroundingPoints(tree, h.position, dist);
              if (sPoints.size() > 3)
                {
                  matrix<double> st = genST(sPoints, h.position);
                  vector<double> eval;
                  vector<vector<double>> evect;
                  Eigenvalue(st, eval, evect);
                  // cout << eval << endl;
                  // check for plane
                  if (eval[2] < 0.2 * eval[1] && eval[1] > 0.5 * eval[0])
                    {
                      // assume plane
                      Vector3d normal(evect[2]);
                      double sum = 0.0;
                      for (const Vector3d& v : sPoints)
                        sum += (v - h.position).dotProduct(normal);
                      double shift = sum / sPoints.size();
                      newXyz[x][y] = h.position + shift * normal;
                      //cout << "X " << shift << " "; cout.flush();
                    }
                  else
                    {
                      //          cout << "p"; cout.flush();
                    }
                }
            }
        }
    }
  for (int y = 0; y < ySize; y++)
    for (int x = 0; x < xSize; x++)
      world[x][y].position = newXyz[x][y];
  if (verbose)
    cout << endl;
}

#if 0
double symRand(double mVal)
{
  return (drand48() - 0.5) * mVal * 2;
}

Vector3d randVector(double mVal)
{
  return Vector3d(symRand(mVal), symRand(mVal), symRand(mVal));
}

void applyNoise(vector<Vector3d>& points, double nVal)
{
  for (auto& p : points)
    {
      p += randVector(nVal);
    }
}

int main(int argc, char** argv)
{
  vector<Vector3d> points;
  matrix<double> st(3, 3);

  for (int x = -10; x <= 10; x++)
    for (int y = -10; y <= 10; y++)
      //      points.push_back(Vector3d(x, y, -0.1 * x - 0.1 * y + 10));
      points.push_back({x * 1.0, y * 1.0, fabs(x * 0.2 + y * (-0.1))});
  applyNoise(points, 0.1);

  Vector3d center = points[points.size() / 2];
  cout << "center: " << center << endl;
  st = genST(points, center, 1.2);
  cout << st << endl;

  vector<double> eval;
  vector<vector<double>> evect;
  Eigenvalue(st, eval, evect);
  cout << eval << endl;
  // check for plane
  if (eval[2] < 0.03 * eval[1] && eval[1] > 0.5 * eval[0])
    {
      cout << "plane" << endl;
      // assume plane
    }
  cout << evect << endl;

  return 0;
}
#endif
