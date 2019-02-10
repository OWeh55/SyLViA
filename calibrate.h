#ifndef CALIBRATE_H
#define CALIBRATE_H

void findMarker(Image& tGray, Image& tMark,
                vector<Contur>& squares, Trafo& tA, Trafo& tB);

constexpr double rasterMM = 22.4;

extern bool verbose;

class LMcalib: public LMFunctor
{
public:
  LMcalib(const vector<Point>& uv,
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

vector<double> calculateCalibration(const vector<Point>& uv,
                                    const vector<double>& u2,
                                    const vector<Vector3d>& xyz);

#endif
