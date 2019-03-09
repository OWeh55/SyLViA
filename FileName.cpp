#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <IceException.h>
#include "FileName.h"

using namespace std;
using ice::IceException;

void split(const string& s, char del, vector<string>& parts)
{
  unsigned int i = 0;
  parts.clear();
  parts.push_back("");
  while (i < s.size())
    {
      if (s[i] != del)
        parts.back() += s[i];
      else
        {
          parts.push_back("");
        }
      i++;
    }
}

void FileName::setPath(const std::string& n)
{
  // set parts of n to path
  // includes filename if available!
  if (n.size() == 0)
    throw IceException("FileName::setPath", "empty pathname");
  absolute = n[0] == pathdel;
  split(n, pathdel, path);
}

FileName::FileName(const std::string& n): spec(""), number(-1)
{
  if (n.size() == 0)
    throw IceException("FileName", "empty filename");

  setPath(n);

  // last path component is name
  if (path.size() > 0)
    {
      // last part of path is (file-)name
      name = path.back();
      path.pop_back();

      size_t lastExtensionDelimiterPosition = name.rfind(".");
      if (lastExtensionDelimiterPosition == std::string::npos)
        extension = "";
      else
        {
          extension = name.substr(lastExtensionDelimiterPosition + 1);
          name.resize(lastExtensionDelimiterPosition);
          splitName();
          /*
          cout << name << endl;
          cout << spec << endl;
          cout << number << endl;
          */
        }
    }
}

void FileName::setName(const string& n)
{
  for (unsigned int i = 0; i < n.size(); ++i)
    if (n[i] == pathdel)
      throw IceException("Filename::setName", "path delimiter in name");
  name = n;
  splitName();
}

void FileName::setExtension(const string& n)
{
  for (unsigned int i = 0; i < n.size(); ++i)
    {
      if (n[i] == extdel)
        throw IceException("Filename::setExtension", "extension delimiter in extension");
      if (n[i] == pathdel)
        throw IceException("Filename::setExtension", "path delimiter in extension");
    }
  extension = n;
}

FileName::FileName(const std::string& path,
                   const std::string& name,
                   const std::string& ext)
{
  setPath(path);
  setName(name);
  setExtension(ext);
}

std::string FileName::getFullName() const
{
  std::string res = getPath();
  if (res != "/" && !res.empty())
    res += '/';
  res += getName();
  if (!extension.empty())
    res += extdel + extension;
  return res;
}

FileName::operator string() const
{
  return getFullName();
}

std::string FileName::getPath() const
{
  string res;
  if (absolute)
    res += '/';
  if (path.size() > 0)
    {
      for (unsigned int i = 0; i < path.size() - 1; ++i)
        res += path[i] + '/';
      res += path.back();
    }
  return res;
}

std::string toString(int n, int len)
{
  if (n < 0)
    throw IceException("FileName", "number is negative");
  string res = to_string(n);
  while (res.size() < len)
    res = '0' + res;
  return res;
}

std::string FileName::getName() const
{
  if (number < 0)
    return getBaseName();
  else
    return getBaseName() + partdel + toString(number, nDigits) + partdel + spec;
}

void FileName::splitName()
{
  vector<string> parts;
  split(name, partdel, parts);
  if (parts.size() > 2) // precondition for names of type "basename_number_spec"
    {
      spec = parts.back();
      parts.pop_back();
      number = stoi(parts.back());
      parts.pop_back();
      name = parts[0];
      for (int i = 1; i < parts.size(); i++)
        name += partdel + parts[i];
    }
}
