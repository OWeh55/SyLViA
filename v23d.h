#ifndef V23D_H
#define V23D_H

#include <vector>
#include <string>

#include <image.h>

// globals for convenience
// video
//extern int nFrames;
extern int xSize;
extern int ySize;

// sequence of pattern

// number of different pattern (positiv+negativ) including black/white marker
extern int sequenceLength;
extern std::string sequenceDescription;

// program organization
extern bool verbose;
extern int debug;

// v2analyse
void analysis(const std::vector<double>& gvs,
              const std::vector<double>& gvds,
              int fps,
              vector<double>& sequenceBoundaries,
              int& cycleLength,
              int& cycleStart);

double graySum(const Image& img);

double absGrayDiff(const Image& img1,
                   const Image& img2);

// v2phases
void calcPhases(const vector<ImageD>& img,
                ImageD& phase,
                Image& mask,
                double minlevelg, double minlevelp);

// v2signal
std::vector<double> powerSpectrum(const std::vector<double>& g);
std::vector<double> autoCorrelation(const std::vector<double>& g);
std::vector<double> autoCorrelationFromPowerSpectrum(const std::vector<double>& ps);
void zeroPadding(std::vector<double>& v, int before, int after);

// v2vector
double findMaxBetween(const std::vector<double>& dg, int i1 = 0, int i2 = -1);
double findMaxAround(const vector<double>& dg, int is, int di);

vector<double> dt(const vector<double>& v);

template<typename T>
T median(vector<T> v)
{
  sort(v.begin(), v.end());
  return v[v.size() / 2];
}

#endif
