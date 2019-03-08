// tests for transform and calibration
#include <iostream>
#include <iomanip>
#include <vector>
#include <image.h>

#include "v2trafo.h"

bool verbose = true;

void output(const Camera&cam,const vector<Vector3d> &xyz,
	    const vector<Point> &uv)
{
  cout << cam.toString(" ") << endl;
  Trafo tr=cam.getTrafo();
  cout << tr.getMatrix() << endl;
  double error2=0;
  for (int i=0;i<xyz.size();i++)
    {
      Vector3d x(xyz[i]);
      Point p(cam.transform(x));
      cout << x << " -> " << p << "   ==   " << uv[i] << "  ";
      double e2=(p - uv[i]).length2();
      cout << sqrt(e2) << endl;
      error2+=e2;
    }
  cout<<"total error: " << sqrt(error2) << endl;
}
  
int main(int argc, char** argv)
{
  try
  {
    Camera cam1;
    //    cam1.set(10000,1,0,0,0);
    vector<Vector3d> xyz;
    vector<Point> uv;
    xyz.push_back(Vector3d(0,0,0)); uv.push_back(Point(0,0));
    xyz.push_back(Vector3d(1,0,0)); uv.push_back(Point(-1,0));
    xyz.push_back(Vector3d(0,1,0)); uv.push_back(Point(0,1));
    xyz.push_back(Vector3d(1,1,0)); uv.push_back(Point(-1,1));
    
    xyz.push_back(Vector3d(0,0,10)); uv.push_back(Point(0,     0));
    xyz.push_back(Vector3d(1,0,10)); uv.push_back(Point(-1.01, 0));
    xyz.push_back(Vector3d(0,1,10)); uv.push_back(Point(0,     1.01));
    xyz.push_back(Vector3d(1,1,10)); uv.push_back(Point(-1.01, 1.01));

    for (int i=0;i<xyz.size();i++)
      {
	xyz[i]*=10;
	uv[i]*=10;
      }

    output(cam1,xyz,uv);
    
    calibrateC(cam1,xyz,uv);

    output(cam1,xyz,uv);
  }
  catch (IceException& ex)
    {
      cout << "not solvable" << endl;
    }
  return OK;
}
