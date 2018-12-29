
#include "run.h"

using namespace std;

void Run::checkPattern(const string &pattern)
{
  int maxSize=40;
  sinIdx=-1;
  cosIdx=-1;
  grayCodeIdx.resize(maxSize);
  for (auto &i:grayCodeIdx)
    i=-1;
  for (int idx=0;idx<pattern.size();idx++)
    {
      char c=pattern[idx];
      if (c=='s')
	sinIdx=idx;
      else if (c=='c')
	cosIdx=idx;
      else if (c>='1' && c<='9')
	grayCodeIdx[c-'1']=idx;
      else if (c>='A' && c<='Z')
	grayCodeIdx[c-'A'+10]=idx;
      else if (c>='a' && c<='z')
	grayCodeIdx[c-'a'+10]=idx;
      else
	throw "Invalid character in pattern description";
    }
  int i=maxSize-1;
  while (grayCodeIdx[i]<0 && i>0)
    i--;
  if (grayCodeIdx[i]<0)
    throw "pattern does not contain graycode images";
  grayCodeIdx.resize(i+1);
  if (sinIdx<0)
    throw "pattern does not contain sinus images";
  if (cosIdx<0)
    throw "pattern does not contain cosinus images";
}

Run::Run(const std::vector<ice::Image> &imgs,
	 double start,double end,
	 const string &patternString):imageSequence(imgs)
{
  checkPattern(patternString);
  grayCodeValues.resize(grayCodeIdx.size());
  int xSize=imgs[0].xsize;
  int ySize=imgs[0].ysize;
  for (auto &g:grayCodeValues)
    g.create(xSize,ySize);
  sinValue.create(xSize,ySize);
  cosValue.create(xSize,ySize);
}
