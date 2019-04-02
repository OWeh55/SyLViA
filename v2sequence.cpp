#include "v2sequence.h"

using namespace std;

int getP(const string& sequence, int& i)
{
  int result;
  if (i >= sequence.size())
    throw "Parameter missing";
  char c = toupper(sequence[i]);
  i++;
  if (c >= '0' && c <= '9')
    {
      result = c - '0';
    }
  else if (c >= 'A' && c <= 'Z')
    result = c - 'A' + 10;
  else
    throw "wrong Parameter";
  return result;
}

void getRange(const string& sequence, int& i, int& first, int& last)
{
  first = getP(sequence, i);
  last = first;
  if (sequence[i] == '-')
    {
      i++;
      last = getP(sequence, i);
    }
}

string translateSequence(const string& sequence, bool noInverse)
{
  string xSequence = "";

  // for compatibility we add a black to white transition if not yet contained
  // therefore we register black and white patterns
  bool hasBlack = false;
  bool hasWhite = false;

  // compile single character
  for (int i = 0; i < sequence.size(); i++)
    {
      char c = sequence[i];
      switch (c)
        {
        case 's':
          xSequence += "s4";
          if (!noInverse)
            xSequence += "S4";
          break;
        case 'c':
          xSequence += "c4";
          if (!noInverse)
            xSequence += "C4";
          break;
        case 'b':
          xSequence += "b0"; // number is dummy here
          hasBlack = true;
          if (!noInverse)
            {
              xSequence += "B0";
              hasWhite = true;
            }
          break;
        case 'w':
          xSequence += "B0"; // number is dummy here
          hasWhite = true;
          if (!noInverse)
            {
              xSequence += "b0";
              hasBlack = true;
            }
          break;
        default:
          if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
            {
              xSequence += "g";
              xSequence += c;
              if (!noInverse)
                {
                  xSequence += "G";
                  xSequence += c;
                }
            }
          else
            throw "unknown char in sequence";
        }
    }

  if (! hasBlack || ! hasWhite)
    xSequence += "b0B0";
  return xSequence;
}
