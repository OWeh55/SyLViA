#ifndef RUN_H
#define RUN_H

#include <vector>
#include <image.h>

class Run
{
private:
  std::vector<ice::ImageD> grayCodeValues;
  ice::ImageD sinValue;
  ice::ImageD cosValue;
  const std::vector<ice::Image>& imageSequence;
  int sinIdx;
  int cosIdx;
  vector<int> grayCodeIdx;
  //
  void checkPattern(const string& pattern);
public:
  Run(const std::vector<ice::Image>& imgs,
      double start, double end,
      const string& patternString);
  void getPhase(ice::ImageD& imgPhase);
};
#endif
