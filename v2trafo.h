#ifndef V2_TRAFO_H
#define V2_TRAFO_H

#include <vector>
#include <image.h>

// 3d to image
class SLModel
{
public:
  virtual ~SLModel() {}
  // get serialized parameters (for storage..)
  virtual std::vector<double> getPara() const;
  virtual void setPara(const std::vector<double>& para);

  // transform 3d point (x,y,z) to coordinates in image (u,v) and phase (u2)
  virtual void transform(const Vector3d& xyz,
                         Point& uv, double& u2) const = 0;

  // reconstruct 3d point from coordinates in image and phase
  virtual void reconstruct(Point uv, double u2,
                           Vector3d& xyz) const = 0;

  // calibrate model based on given correspondences
  // (x,y,z) < -- > (u,v), u2
  virtual void computeParameter(const std::vector<Point>& uv,
                                const std::vector<double>& u2,
                                const std::vector<Vector3d>& xyz,
                                char mode) = 0;

  // primarily for debug
  // specialized methods should use an understandable form
  virtual void printPara() const;
protected:
  std::vector<double> para;
};

class SLModel1 : public SLModel
{
public:
  virtual ~SLModel1() {}
  // transform 3d point (x,y,z) to coordinates in image (u,v) and phase (u2)
  virtual void transform(const Vector3d& xyz,
                         Point& uv, double& u2) const;

  // reconstruct 3d point from coordinates in image and phase
  virtual void reconstruct(Point uv, double u2,
                           Vector3d& xyz) const;

  // calibrate model based on given correspondences
  // (x,y,z) < -- > (u,v), u2
  virtual void computeParameter(const std::vector<Point>& uv,
                                const std::vector<double>& u2,
                                const std::vector<Vector3d>& xyz,
                                char mode);

  virtual void printPara() const;
};

class SLModel2 : public SLModel
{
public:
  SLModel2();
  virtual ~SLModel2() {}
  static constexpr int nParams = 14;
  // transform 3d point (x,y,z) to coordinates in image (u,v) and phase (u2)
  virtual void transform(const Vector3d& xyz,
                         Point& uv, double& u2) const;

  // reconstruct 3d point from coordinates in image and phase
  virtual void reconstruct(Point uv, double u2,
                           Vector3d& xyz) const;

  // calibrate model based on given correspondences
  // (x,y,z) < -- > (u,v), u2
  virtual void computeParameter(const std::vector<Point>& uv,
                                const std::vector<double>& u2,
                                const std::vector<Vector3d>& xyz,
                                char mode);

  virtual void printPara() const;
  virtual void setPara(const vector<double>& p) override;
private:
  void setCamsFromPara() const;
  mutable bool camsValid;
  mutable Camera camUV;
  mutable Camera camU2;
};

extern bool verbose;
#endif
