#include <image.h>

#include "v2trafo.h"
#include "calibrator.h"

void SLModel2::setPara(const vector<double>& p)
{
  para = p;
  camsValid = false;
  setCamsFromPara();
}

SLModel2::SLModel2(): camsValid(false), camUV(1), camU2(1)
{
  para.resize(nParams * 2);
  vector<double> pUV = camUV.makeVectorDouble();
  for (int i = 0; i < pUV.size(); i++)
    para[i] = pUV[i];
  /*
  para[CAM_FOCAL_LENGTH] = 2500;
  para[CAM_GAMMA] = M_PI;
  para[CAM_ALPHA] = M_PI;
  para[CAM_DZ] = -1000;
  */
  vector<double> pU2 = camU2.makeVectorDouble();
  for (int i = 0; i < pU2.size(); i++)
    para[i + nParams] = pU2[i];
  setCamsFromPara();
  // cout << camUV.getTrafo().getMatrix() << endl;
}

void SLModel2::setCamsFromPara() const
{
  // we assume 11 camera parameters and 3 parameters for Distortion1
  if (!camsValid)
    {
      if (para.size() != 2 * nParams)
        throw IceException("SLModel2::setPara", "wrong number of parameter");

      vector<double> p(nParams);
      for (int i = 0; i < nParams; i++)
        p[i] = para[i];
      camUV.set(p);
      // cout << camUV.getTrafo().getMatrix() << endl;
      for (int i = 0; i < nParams; i++)
        p[i] = para[i + nParams];
      camU2.set(p);
      camsValid = true;
    }
}

void SLModel2::transform(const Vector3d& xyz,
                         Point& uv, double& u2) const
{
  setCamsFromPara();
  uv = camUV.transform(xyz);
  Point u2v = camU2.transform(xyz);
  u2 = u2v.x;
}

void SLModel2::reconstruct(Point uv, double u2,
                           Vector3d& xyz) const
{
  cerr << "not implemented" << endl;
}

void SLModel2::computeParameter(const vector<Point>& uv,
                                const vector<double>& u2,
                                const vector<Vector3d>& xyz,
                                char model)
{
  LMCalib  f(*this, uv, u2, xyz);
  LMSolver lm(f);
  vector<int> selected{CAM_FOCAL_LENGTH, CAM_PRINCIPAL_POINT_U, CAM_PRINCIPAL_POINT_V,
                       CAM_DX, CAM_DY, CAM_DZ, CAM_ALPHA, CAM_BETA, CAM_GAMMA,
                       CAM_FOCAL_LENGTH + nParams, CAM_PRINCIPAL_POINT_U + nParams,
                       CAM_DX + nParams, CAM_DY + nParams, CAM_DZ + nParams,
                       CAM_ALPHA + nParams, CAM_BETA + nParams, CAM_GAMMA + nParams
                      };
  lm.solve(para, selected);
  // cout << lm.getInfo() << endl;
  camsValid = false;
  setCamsFromPara();
  // cout << camUV.getTrafo().getMatrix() << endl;
}

void SLModel2::printPara() const
{
  setCamsFromPara();
  cout << camUV.toString() << endl;
  // cout << camUV.getTrafo().getMatrix() << endl;
  cout << camU2.toString() << endl;
}
