/*
 * 
 */
#ifndef V23D_TRIANGLE_H
#define V23D_TRIANGLE_H

namespace ice
{
  class TrianglePL
  {
  public:
    /** standard constructor
     */
    TrianglePL(const vector<Point> &pl, int i1, int i2, int i3):
      i1(i1), i2(i2), i3(i3), pl(&pl)
    {

    }
    
    /*
    ** get Corner
     */
#define FNAME "Triangle::getCorner"
    Point getCorner(int i) const
    {
      switch (i)
        {
        case 0:
          return P1();
        case 1:
          return P2();
        case 2:
          return P3();
        default:
          throw IceException(FNAME, M_WRONG_INDEX);
        }
    }
#undef FNAME

    const Point& P1() const
    {
      return (*pl)[i1];
    }
    const Point& P2() const
    {
      return (*pl)[i2];
    }
    const Point& P3() const
    {
            return (*pl)[i3];
    }

    const ice::Triangle getTriangle() const
    {
      return ice::Triangle( (*pl)[i1], (*pl)[i2], (*pl)[i3]);
    }

  private:
    int i1,i2,i3;
    const vector<Point> *pl;
  };
}
#endif
