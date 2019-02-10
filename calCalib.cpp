#include <image.h>
#include "calibrate.h"

LMcalib::LMcalib(const vector<Point>& uv,
                 const vector<double>& u2,
                 const vector<Vector3d>& xyz):
  uv(uv), u2(u2), xyz(xyz), nRefs(uv.size()) {}

int LMcalib::getDimension() const
{
  return nRefs * 2;
}

int LMcalib::operator()(const vector<double>& p, vector<double>& result) const
{
  int d = 0;
  for (int i = 0; i < nRefs; i++)
    {
      double u = uv[i].x;
      double v = uv[i].y;
      double x = xyz[i].x;
      double y = xyz[i].y;
      double z = xyz[i].z;
      double ud2 = u2[i];
      double uc = p[0] * x + p[1] * y + p[2] * z + p[3];
      double vc = p[4] * x + p[5] * y + p[6] * z + p[7];
      double wc = p[8] * x + p[9] * y + p[10] * z + 1;
      double uuc = uc / wc;
      double vvc = vc / wc;
      result[d++] = u - uuc;
      result[d++] = v - vvc;

      double up = p[11] * x + p[12] * y + p[13] * z + p[14];
      double wp = p[15] * x + p[16] * y + p[17] * z + 1;
      double uu2 = up / wp;
      result[d++] = ud2 - uu2;
    }
  return 1;
}

vector<double> calculateCalibration(const vector<Point>& uv,
                                    const vector<double>& u2,
                                    const vector<Vector3d>& xyz)
{
  LMcalib calibFunctor(uv,u2,xyz);
  vector<double> cData(18,0.0);
  cData[0]=1;
  cData[5]=1;
  cData[11]=1;
  LMSolver lms(calibFunctor);
  lms.solve(cData);
  if (verbose)
    {
  cout << "Nach " << lms.getNIterations() << " Iterationen " << endl;
  cout << "Kode für Beendigung: " << lms.getInfo() << endl;
  cout << "Restfehler: " << lms.getErrorValue() << endl;
    }
  return cData;
}
