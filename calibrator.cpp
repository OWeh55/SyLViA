#include "calibrator.h"

LMcalib::LMcalib(SLModel& model,
                 const vector<Point>& uv,
                 const vector<double>& u2,
                 const vector<Vector3d>& xyz):
  slModel(model), uv(uv), u2(u2), xyz(xyz), nRefs(uv.size()) {}

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
      slModel.setPara(p);
      slModel.transform(xyz[i], uvc, u2c);

      Point delta = uvc - uv[i];
      result[d++] = delta.x;
      result[d++] = delta.y;

      result[d++] = u2c - u2[i];
    }
  return 1;
}
