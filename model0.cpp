#include <image.h>

#include "v2trafo.h"

void SLModel::printPara() const
{
  cout << fixed << setprecision(4);
  cout << "--" << endl;
  for (int i = 0; i < para.size(); i++)
    {
      cout << para[i] << " ";
      if (i % 6 == 5)
        cout << endl;
    }
  cout << endl << "--" << endl;
}

void SLModel::setPara(const std::vector<double>& para)
{
  this->para = para;
}

std::vector<double> SLModel::getPara() const
{
  return para;
}
