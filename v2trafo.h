#ifndef V2_TRAFO_H
#define V2_TRAFO_H

// 3d to image
void transform(const vector<double>& para,
               const Vector3d& xyz,
               Point& uv, double& u2);

void reconstruct(const vector<double>& para,
                 Point uv, double u2,
                 Vector3d& xyz);

// calibration (estimate parameters)
class LMcalibP: public LMFunctor
{
public:
  LMcalibP(const vector<Point>& uv,
           const vector<double>& u2,
           const vector<Vector3d>& xyz);
  int getDimension() const;
  int operator()(const vector<double>& p, vector<double>& result) const;

private:
  const vector<Point>& uv;
  const vector<double>& u2;
  const vector<Vector3d>& xyz;
  int nRefs;
};

vector<double> computeParameter(const vector<Point>& uv,
                                const vector<double>& u2,
                                const vector<Vector3d>& xyz,
                                char mode);

void calibrateC(Camera &cam, const vector<Vector3d> &xyz, const vector<Point> &uv);

extern bool verbose;

void printPara(const vector<double>& p);

#endif
