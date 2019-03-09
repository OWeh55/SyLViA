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
  LMcalib  f(*this, uv, u2, xyz);
  LMSolver lm(f);
  lm.solve(para);
  camsValid = false;
  setCamsFromPara();
}

void SLModel2::printPara() const
{
  setCamsFromPara();
  cout << camUV.toString() << endl;
  cout << camU2.toString() << endl;
}
