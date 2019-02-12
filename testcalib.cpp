// tests for transform and calibration
#include <iostream>
#include <iomanip>
#include <vector>
#include <image.h>

#include "v2trafo.h"

bool verbose = true;

int main(int argc, char** argv)
{
  vector<double> para(18, 0.0);
  para[0] = 2;
  para[1] = 0.5;
  para[3] = 100;
  para[5] = 2.1;
  para[4] = -0.5;
  para[7] = 50;
  para[10] = 0.01;
  para[11] = 1;
  para[17] = 0.01;
  printPara(para);
  // test calibration
  vector<Vector3d> vxyz;
  vector<Point> vuv;
  vector<double> vu2;
  for (int z = 0; z < 10; z++)
    for (int y = 0; y < 100; y += 10)
      for (int x = 0; x < 100; x += 10)
        {
          double u2;
          Point uv;
          Vector3d xyz(x, y, z);
          transform(para, xyz, uv, u2);
          vxyz.push_back(xyz);
          vuv.push_back(uv);
          vu2.push_back(u2);
        }
  vector<double> epara = calculateParameter(vuv, vu2, vxyz);
  printPara(epara);
  // reconstruction
  for (int i = 0; i < vxyz.size(); i++)
    {
      Vector3d xyz;
      try
        {
          reconstruct(epara, vuv[i], vu2[i], xyz);
          cout << xyz << " ? " << vxyz[i] << " " << (xyz - vxyz[i]).length() << endl;
        }
      catch (IceException& ex)
        {
          cout << "not solvable" << endl;
        }
    }
  return OK;
}
