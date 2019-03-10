#ifndef CALIBRATOR_H
#define CALIBRATOR_H

#include "v2trafo.h"

class LMCalib: public LMFunctor
{
public:
  LMCalib(SLModel& slModel,
          const vector<Point>& uv,
          const vector<double>& u2,
          const vector<Vector3d>& xyz);
  int getDimension() const;
  int operator()(const vector<double>& p, vector<double>& result) const;

private:
  SLModel& slModel;
  const vector<Point>& uv;
  const vector<double>& u2;
  const vector<Vector3d>& xyz;
  int nRefs;
};
#endif
