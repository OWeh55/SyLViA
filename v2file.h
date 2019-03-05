#ifndef _V2_FILE_H
#define _V2_FILE_H

#include <image.h>
#include <vector>
#include <string>

void fileNames(const std::string& baseFileName, std::string& textureFN, std::string& phaseFN, std::string& calFN);

void scanVideo(const std::string& fn,
               std::vector<double>& gsum,
               std::vector<double>& dgsum,
               int& fps,
               int first, int last,
               int colorMode,
               const ice::Window& readWindow);

void readSequence(VideoFile& v,
                  double leftBoundary, double rightBoundary,
                  int sequenceLength,
                  int colorMode,
                  const Window& readWindow,
                  int nFrames,
                  vector<ImageD>& seq,
                  ColorImage& cImg,
                  ColorImage& deviation);

void writePlotFile(const std::string& name, const std::vector<double>& v);
void writePlotFile(const std::string& name, const std::vector<double>& v1,
                   const std::vector<double>& v2);

void writeCalib(const std::string& name,
                const std::vector<double>& v, char calibrationModel);
bool readCalib(const std::string& name,
               std::vector<double>& v, char& calibrationModel);

#endif
