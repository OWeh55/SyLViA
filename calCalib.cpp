#include <image.h>
#include "calibrate.h"

LMcalib::LMcalib(const vector<Point>& uv,
                 const vector<double>& u2,
                 const vector<Vector3d>& xyz):
  uv(uv), u2(u2), xyz(xyz), nRefs(uv.size()), nSteps(0) {}

int LMcalib::getDimension() const
{
  return nRefs * 3;
}

int LMcalib::operator()(const vector<double>& p, vector<double>& result) const
{
  vector<Point> points;
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
      points.push_back(Point(uuc, vvc));
      result[d++] = u - uuc;
      result[d++] = v - vvc;

      double up = p[11] * x + p[12] * y + p[13] * z + p[14];
      double wp = p[15] * x + p[16] * y + p[17] * z + 1;
      double uu2 = up / wp;
      result[d++] = ud2 - uu2;
    }
#if 0
  nSteps++;
  if (nSteps % 400 == 0)
    {
      Image debugI;
      debugI.create(1200, 900);
      debugI.set(0);
      Show(ON, debugI);
      for (auto p : points)
        Marker(5, p, 255, 10, debugI);
      GetChar();
      Show(OFF, debugI);
    }
  d = 0;
  for (int k = 0; k < 3; k++)
    {
      for (int i = 0; i < 4; i++)
        if (d < 11)
          cout << p[d++] << " ";
      cout << endl;
    }
  cout << "--------" << endl;

  for (int k = 0; k < 2; k++)
    {
      for (int i = 0; i < 4; i++)
        if (d < 18)
          cout << p[d++] << " ";
      cout << endl;
    }
  cout << "--------" << endl;
  for (int i = 0; i < result.size(); i++)
    {
      cout << result[i] << " ";
      if (i % 3 == 2)
        cout << endl;
    }
#endif
  /*
  */
  return 1;
}

vector<double> calculateCalibration(const vector<Point>& uv,
                                    const vector<double>& u2,
                                    const vector<Vector3d>& xyz)
{
  LMcalib calibFunctor(uv, u2, xyz);
  vector<double> cData(18, 0.0);
  cData[0] = 2.1;
  cData[3] = 500;
  cData[5] = 2.1;
  cData[7] = 92;
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
