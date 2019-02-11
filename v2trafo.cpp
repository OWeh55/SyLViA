
#include <image.h>

#include "v2trafo.h"

void transform(const vector<double>& para,
               const Vector3d& xyz,
               Point& uv, double& u2)
{
  double uc = para[0] * xyz.x + para[1] * xyz.y + para[2] * xyz.z + para[3];
  double vc = para[4] * xyz.x + para[5] * xyz.y + para[6] * xyz.z + para[7];
  double wc = para[8] * xyz.x + para[9] * xyz.y + para[10] * xyz.z + 1;
  uv.x = uc / wc;
  uv.y = vc / wc;

  double up = para[11] * xyz.x + para[12] * xyz.y + para[13] * xyz.z + para[14];
  double wp = para[15] * xyz.x + para[16] * xyz.y + para[17] * xyz.z + 1;
  u2 = up / wp;
}

void reconstruct(const vector<double>& para,
                 Point uv, double u2,
                 Vector3d& xyz)
{
  vector<double> xyzV(3);
  vector<double> inhV(3);
  matrix<double> equV(3, 3);

  // we solve the linearized form of the system of equations

  equV[0][0] = para[0] - para[8] * uv.x;
  equV[0][1] = para[1] - para[9] * uv.x;
  equV[0][2] = para[2] - para[10] * uv.x;

  equV[1][0] = para[4] - para[8] * uv.y;
  equV[1][1] = para[5] - para[9] * uv.y;
  equV[1][2] = para[6] - para[10] * uv.y;

  equV[2][0] = para[11] - para[15] * u2;
  equV[2][1] = para[12] - para[16] * u2;
  equV[2][2] = para[13] - para[17] * u2;

  inhV[0] = uv.x - para[3];
  inhV[1] = uv.y - para[7];
  inhV[2] = u2 - para[14];

  xyzV = solveLinearEquation(equV, inhV);
  xyz.x = xyzV[0];
  xyz.y = xyzV[1];
  xyz.z = xyzV[2];
}


LMcalib::LMcalib(const vector<Point>& uv,
                 const vector<double>& u2,
                 const vector<Vector3d>& xyz):
  uv(uv), u2(u2), xyz(xyz), nRefs(uv.size()) {}

int LMcalib::getDimension() const
{
  return nRefs * 3;
}

int LMcalib::operator()(const vector<double>& p, vector<double>& result) const
{
  int d = 0;
  for (int i = 0; i < nRefs; i++)
    {
      Point uvc;
      double u2c;
      transform(p, xyz[i], uvc, u2c);

      Point delta = uvc - uv[i];
      result[d++] = delta.x;
      result[d++] = delta.y;

      result[d++] = u2c - u2[i];
    }
  return 1;
}

vector<double> calculateParameter(const vector<Point>& uv,
                                  const vector<double>& u2,
                                  const vector<Vector3d>& xyz)
{
  LMcalib calibFunctor(uv, u2, xyz);
  vector<double> cData(18, 0.0);
  cData[0] = 2;
  cData[3] = 500;
  cData[5] = 2;
  cData[7] = 200;
  cData[11] = 1.0;
  LMSolver lms(calibFunctor);
  lms.solve(cData);
  if (verbose)
    {
      cout << "after " << lms.getNIterations() << " iterations " << endl;
      cout << "code: " << lms.getInfo() << endl;
      cout << "    " << LMDifMessage(lms.getInfo()) << endl;
      cout << "remaining error: " << lms.getErrorValue() << endl;
    }
  return cData;
}
