#ifndef _FILENAME_H
#define _FILENAME_H

#include <vector>
#include <string>

class FileName
{
public:
  static const char pathdel = '/';
  static const char extdel = '.';
  static const char partdel = '_';

  FileName(): absolute(false) {};

  FileName(const std::string& filename);

  FileName(const std::string& path,
           const std::string& name,
           const std::string& ext);

  std::string getFullName() const;
  operator std::string() const;

  std::string getName() const;
  std::string getBaseName() const
  {
    return name;
  }
  std::string getSpec() const;
  int getNumber() const;
  std::string getPath() const;
  std::string getExtension() const
  {
    return extension;
  }

  void setName(const std::string& n);
  void setBaseName(const std::string& n);
  void setSpec(const std::string& n)
  {
    spec = n;
  }
  void setNumber(int n)
  {
    number = n;
  }
  void setPath(const std::string& p);
  void setExtension(const std::string& x);

private:
  void splitName();
  const int nDigits = 3;
  bool absolute;
  std::vector<std::string> path;
  std::string name;
  std::string spec;
  int number;
  std::string extension;
};

#endif
