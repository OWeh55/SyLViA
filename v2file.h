#ifndef _V2_FILE_H
#define _V2_FILE_H

#include <vector>
#include <string>

void fileNames(const std::string& baseFileName, std::string& textureFN, std::string& phaseFN, std::string& calFN);

void scanVideo(const std::string& fn,
               vector<double>& gsum,
               vector<double>& dgsum,
               int& fps,
               int first, int last,
               int colorMode,
               const Window& readWindow);

void readSequence(VideoFile& v,
                  double leftBoundary, double rightBoundary,
                  int sequenceLength,
                  int colorMode,
                  const Window& readWindow,
                  vector<ImageD>& seq,
                  ColorImage& cImg);

void writePlotFile(const std::string& name, const std::vector<double>& v);
void writePlotFile(const std::string& name, const std::vector<double>& v1,
                   const std::vector<double>& v2);

void writeCalib(const std::string& name, const std::vector<double>& v);
bool readCalib(const std::string& name, std::vector<double>& v);

#endif
