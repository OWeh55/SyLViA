#include "v2file.h"
#include "v23d.h"

// functions to calculate phase from captured images

// plot wrapped phase and strip number
//#define PDEBUG 99

using namespace std;

int getGrayCode(int i)
{
  return (i / 2) ^ i;
}

int decodeGrayCode(const vector<ImageD>& seq,
                   ImageD& phase,
                   Image& mask,
                   double minlevel)
{
  // create decoder table
  int ungray[512];
  for (int i = 0; i < 512; i++)
    {
      ungray[getGrayCode(i)] = i;
    }

  int xSize = seq[0].xsize;
  int ySize = seq[0].ysize;

  for (int y = 0; y < ySize; ++y)
    for (int x = 0; x < xSize; ++x)
      {
        int grayCodeValue = 0;
        int bit = 1; // LSB first
        int nError = 0;

        for (int i = 2; i < seq.size(); i += 1, bit <<= 1)
          {
            double difg = seq[i].getPixel(x, y);
            if (abs(difg) < minlevel)
              nError++;
            grayCodeValue += difg > 0 ? bit : 0;
          }
        // we accept one uncertain bit assuming change between two values
        //        if (nError < 2) // Graycode valid ?
        // we avoid uncertain pixels requiring zero errors
        if (nError == 0)
          {
            phase.setPixel(x, y, ungray[grayCodeValue]);
          }
        else
          {
            phase.setPixel(x, y, 0.0);
            mask.setPixel(x, y, 1); // invalid stripNr
          }
      }
  return 0;
}

void calcPhases(const vector<ImageD>& img,
                ImageD& phase,
                Image& mask,
                double minlevelg,
                double minlevelp)
{
  int xSize = img[0].xsize;
  int ySize = img[0].ysize;

  phase.create(xSize, ySize);
  phase.set(0.0);
  mask.create(xSize, ySize);
  mask.set(0); // all valid

  decodeGrayCode(img, phase, mask, minlevelg);

  double minlevel2 = minlevelp * minlevelp;

  double phaseMin = numeric_limits<double>::max() ;
  double phaseMax = - numeric_limits<double>::max() ;
#if PDEBUG
  vector<double> dphase(xSize, 0);
  vector<double> dnr(xSize, 0);
#endif
  for (int y = 0; y < ySize; ++y)
    for (int x = 0; x < xSize; ++x)
      {
        if (mask.getPixel(x, y) == 0)
          {
            double c = -img[0].getPixel(x, y);
            double s = img[1].getPixel(x, y);
            double fic =  atan2(c, s) + M_PI; // phase 0..2*pi
            fic = fmod(fic + (1.75 * M_PI), 2 * M_PI); //
#if PDEBUG
            if (y == ySize / 2)
              dphase[x] = fic * 10;
#endif
            int snr = phase.getPixel(x, y);
#if PDEBUG
            if (y == ySize / 2)
              dnr[x] = snr % 64;
#endif
            int fih = snr / 2;
            int fil = snr % 2;

            if ((fic > 3 * M_PI / 2) && (fil == 0)) fih--; // fil=1
            if ((fic < M_PI / 2) && (fil == 1)) fih++; // fil=0

            double ph =  fih + (fic / (2 * M_PI));
            ph += 1; // make sure phase is positive
            phase.setPixel(x, y, ph);
            if ((s * s + c * c) >= minlevel2)
              {
                if (ph < phaseMin)
                  phaseMin = ph;
                if (ph > phaseMax)
                  phaseMax = ph;
              }
            else
              {
                mask.setPixel(x, y, mask.getPixel(x, y) | 2);
              }
          }
      }
  phase.setLimits(phaseMin, phaseMax);
#if PDEBUG
  writePlotFile("phase.gp", dphase);
  writePlotFile("number.gp", dnr);
#endif
  if (debug & 8)
    {
      Show(OVERLAY, mask);
      Show(GRAY, phase);
      GetChar();
      Show(OFF, mask);
      Show(OFF, phase);
    }
}
