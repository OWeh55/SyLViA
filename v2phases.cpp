#include "v23d.h"

// functions to calculate phase from captured images

using namespace std;

int getGrayCode(int i)
{
  return (i / 2) ^ i;
}

int decodeGrayCode(const vector<ImageD>& seq,
                   ImageD& phase,
                   Image& mask,
                   double minlevel = 2)
{
  // create decoder table
  int ungray[512];
  for (int i = 0; i < 512; i++)
    {
      ungray[getGrayCode(i)] = i;
    }
  /*
  for (int i = 2; i < seq.size(); i++)
    Show(GRAY,const_cast<ImageD&>(seq[i]),"image "+to_string(i));
  */
  for (int y = 0; y < ySize; ++y)
    for (int x = 0; x < xSize; ++x)
      {
        int gc = 0;
        int bit = 1; // MSB first
        bool good = true;

        //        for (int i = 2; i < seq.size() && good; i += 2, bit >>= 1)
        for (int i = 2; i < seq.size() && good; i += 1, bit <<= 1)
          {
            double difg = seq[i].getPixel(x, y);
            good = abs(difg) >= minlevel;
            gc += difg > 0 ? bit : 0;
          }

        if (good) // Graycode valid
          {
            phase.setPixel(x, y, ungray[gc]);
          }
        else
          {
            phase.setPixel(x, y, 0.0);
            mask.setPixel(x, y, 1); // invalid stripNr
          }
      }
  /*
  GetChar();
  for (int i = 2; i < seq.size(); i++)
    Show(OFF,const_cast<ImageD&>(seq[i]));
  */
  return 0;
}

int calcPhases(const vector<ImageD>& img,
               ImageD& phase,
               Image& mask,
               double minlevel)
{
  phase.create(xSize, ySize);
  phase.set(0.0);
  mask.create(xSize, ySize);
  mask.set(0); // all valid

  //  Show(OVERLAY, mask);
  Show(GRAY, phase);

  decodeGrayCode(img, phase, mask, minlevel);

  for (int y = 0; y < ySize; ++y)
    for (int x = 0; x < xSize; ++x)
      {
        if (mask.getPixel(x, y) == 0)
          {
            double c = -img[0].getPixel(x, y);
            double s = img[1].getPixel(x, y);

            double fic = atan2(c, s) + M_PI;
            int snr = phase.getPixel(x, y);
            int fih = snr / 4;
            int fil = snr % 4;

            // cout << fih << " " << fil << " " << fic << endl;

            if ((fic > 3 * M_PI / 2) && (fil == 0)) fih--; // fil=3
            if ((fic < M_PI / 2) && (fil == 3)) fih++; // fil=0

            // cout << fih << " " << fic << endl;
            phase.setPixel(x, y, fih + (fic / (2 * M_PI)));
            if ((s * s + c * c) < minlevel)
              {
                mask.setPixel(x, y, 2);
              }
          }
      }
  phase.adaptLimits();
  GetChar();
  //  Show(OFF, mask);
  Show(OFF, phase);
  return 0;
}

void calcPhases(const std::vector<std::vector<ImageD>>& extractedPattern,
                std::vector<ImageD>& phaseImages,
                std::vector<Image>& maskImages,
                double minlevel)
{
  int nSequences = extractedPattern.size();
  phaseImages.resize(nSequences);
  maskImages.resize(nSequences);
  for (int i = 0; i < nSequences; i++)
    calcPhases(extractedPattern[i], phaseImages[i], maskImages[i], minlevel);
}
