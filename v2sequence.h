#ifndef V2_SEQUENCE
#define V2_SEQUENCE
/*
 * functions for handling description of pattern sequence
 */
#include <string>

int getP(const std::string& sequence, int& i);
std::string translateSequence(const std::string& sequence, bool noInverse);

#endif
