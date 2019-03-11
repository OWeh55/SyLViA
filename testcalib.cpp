// tests for transform and calibration
#include <iostream>
#include <iomanip>
#include <vector>
#include <image.h>

#include "v2trafo.h"

bool verbose = true;

int main(int argc, char** argv)
{
  SLModel* model = new SLModel2();
  vector<double> para(SLModel2::nParams * 2, 0.0);
  para[CAM_FOCAL_LENGTH] = 800;
  para[CAM_ASPECT_RATIO] = 1;
  // para[CAM_BETA]=0.1;
  para[CAM_DZ] = 1000;

  para[CAM_FOCAL_LENGTH + SLModel2::nParams] = 1000;
  para[CAM_ASPECT_RATIO + SLModel2::nParams] = 1;
  para[CAM_DZ + SLModel2::nParams] = -1000;
  model->setPara(para);
  cout << "parameter for given transformation" << endl;
  model->printPara();

  // compute references
  vector<Vector3d> vxyz;
  vector<Point> vuv;
  vector<double> vu2;
  for (int z = 0; z < 100; z += 10)
    for (int y = 0; y < 100; y += 10)
      for (int x = 0; x < 100; x += 10)
        {
          double u2;
          Point uv;
          Vector3d xyz(x, y, z);
          model->transform(xyz, uv, u2);
          vxyz.push_back(xyz);
          vuv.push_back(uv);
          vu2.push_back(u2);
        }

  // reset parameters
  delete model;
  model = new SLModel2();
  //  model->printPara();
  // calibrate model
  model->computeParameter(vuv, vu2, vxyz, 'c');
  cout << "estimated parameters" << endl;
  model->printPara();
#if 0
  // reconstruction
  for (int i = 0; i < vxyz.size(); i++)
    {
      Vector3d xyz;
      try
        {
          model->reconstruct(vuv[i], vu2[i], xyz);
          cout << xyz << " ? " << vxyz[i] << " " << (xyz - vxyz[i]).length() << endl;
        }
      catch (IceException& ex)
        {
          cout << "not solvable" << endl;
        }
    }
#endif
  delete model;
  return OK;
}
