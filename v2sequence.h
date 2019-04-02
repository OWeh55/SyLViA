#ifndef V2_SEQUENCE
#define V2_SEQUENCE
/*
 * functions for handling description of pattern sequence
 */
#include <vector>
#include <string>

class pattern_t
{
public:
  char type;
  int parameter;
  pattern_t(): type(' '), parameter(0) {}
  pattern_t(char t, int p): type(t), parameter(p) {}
  operator std::string() const
  {
    return type + std::to_string(parameter);
  }
};

int getP(const std::string& sequence, int& i);
std::string translateSequence(const std::string& sequence, bool noInverse);
std::vector<pattern_t> vSequence(const std::string& sequence, bool noInverse);

#endif
