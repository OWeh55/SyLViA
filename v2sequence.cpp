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

int intP(char c)
{
  c = toupper(c);
  if (c >= '0' && c <= '9')
    return c - '0';
  else if (c >= 'A' && c <= 'Z')
    return 10 + c - 'A';
  else
    throw "wrong character in parameter character";
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

vector<pattern_t> vSequence(const string& sequence, bool noInverse)
{
  vector<pattern_t> result;

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
          result.push_back(pattern_t('s', 4));
          if (!noInverse)
            result.push_back(pattern_t('S', 4));
          break;
        case 'c':
          result.push_back(pattern_t('c', 4));
          if (!noInverse)
            result.push_back(pattern_t('C', 4));
          break;
        case 'b':
          result.push_back(pattern_t('b', 0)); // number is dummy here
          hasBlack = true;
          if (!noInverse)
            {
              result.push_back(pattern_t('B', 0));
              hasWhite = true;
            }
          break;
        case 'w':
          result.push_back(pattern_t('B', 0));
          hasWhite = true;
          if (!noInverse)
            {
              result.push_back(pattern_t('b', 4));
              hasBlack = true;
            }
          break;
        default:
          if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))
            {
              result.push_back(pattern_t('g', intP(c)));
              if (!noInverse)
                {
                  result.push_back(pattern_t('G', intP(c)));
                }
            }
          else
            throw "unknown char in sequence";
        }
    }

  if (! hasBlack || ! hasWhite)
    {
      result.push_back(pattern_t('b', 0));
      result.push_back(pattern_t('B', 0));
    }
  return result;
}
