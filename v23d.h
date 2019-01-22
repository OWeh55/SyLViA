#ifndef V23D_H
#define V23D_H

#include <vector>
#include <string>

#include <image.h>

// globals for convenience
// video
extern int nFrames;
extern int xSize;
extern int ySize;

// sequence of pattern

// number of different pattern (positiv+negativ) including black/white marker
extern int sequenceLength;
extern std::string sequenceDescription;

// program organization
extern bool verbose;
extern int debug;

// v2file.cpp
void readImages(const std::string& fn,
                std::vector<ice::ColorImage>& ivector, int& fps,
                int first, int last);

void readImages(const string& fn,
                std::vector<Image>& ivector, int& fps,
                int first, int last,
                int colorMode,
                const Window& window);

void scanVideo(const string& fn,
               vector<double>& gsum,
               vector<double>& dgsum,
               int& fps,
               int first, int last,
               int colorMode,
               const Window& readWindow);

void writePlotFile(const std::string& name, const std::vector<double>& v);
void writePlotFile(const std::string& name, const std::vector<double>& v1, const std::vector<double>& v2);

// v2analyse
void analysis(const std::vector<ice::Image>& cv,
              int fps,
              vector<double>& sequenceBoundaries,
              int& cycleLength,
              int& cycleStart);

void analysis(const std::vector<double>& gvs,
              const std::vector<double>& gvds,
              int fps,
              vector<double>& sequenceBoundaries,
              int& cycleLength,
              int& cycleStart);

double graySum(const Image& img);

double absGrayDiff(const Image& img1,
                   const Image& img2);

// v2extract
void extractPattern(const std::vector<ice::ColorImage>& cv,
                    vector<double>& sequenceBoundaries,
                    std::vector<std::vector<ImageD>>& extractedPattern);

void extractPattern(const std::vector<ice::Image>& cv,
                    vector<double>& sequenceBoundaries,
                    std::vector<std::vector<ImageD>>& extractedPattern);

// v2phases
void calcPhases(const std::vector<std::vector<ImageD>>& extractedPattern,
                std::vector<ImageD>& phaseImages,
                std::vector<Image>& maskImages,
                double minlevel);

// v2signal
std::vector<double> powerSpectrum(const std::vector<double>& g);
std::vector<double> autoCorrelation(const std::vector<double>& g);
std::vector<double> autoCorrelationFromPowerSpectrum(const std::vector<double>& ps);
void zeroPadding(std::vector<double>& v, int before, int after);

// v2vector
double findMaxBetween(const std::vector<double>& dg, int i1 = 0, int i2 = -1);
double findMaxAround(const vector<double>& dg, int is, int di);

vector<double> dt(const vector<double>& v);

#endif
