#include <string>
#include <vector>
#include <iostream>

#include <image.h>
#include "v2polygon.h"
#include "calibrate.h"

vector<Point> makePointList(const unsigned char* xy, double xPos, double yPos)
{
  // generate pointlist with x/y coordinates in mm
  double factor = markerSize / xySize * rasterMM;

  Point lo(xPos * rasterMM, yPos * rasterMM);
  IPoint start(xy[0], xy[1]);
  vector<Point> pl;
  Point startd(start);
  startd *= factor;
  startd += lo;

  pl.push_back(startd);
  int idx = 2;
  IPoint p(xy[idx], xy[idx + 1]);
  while (p != start)
    {
      Point dp(p);
      dp *= factor;
      dp += lo;
      pl.push_back(dp);
      idx += 2;
      p = IPoint(xy[idx], xy[idx + 1]);
    }
  return pl;
}

void drawReferences(const Image& img, const ObjectMatcher& om)
{
  vector<Point> pl1;
  vector<Point> pl2;
  om.getOrderedLists(pl1, pl2);
  for (int i = 0; i < pl1.size(); i++)
    {
      Line(pl1[i], pl2[i], 3, img);
    }
}

void findMarker(Image& tGray, Image& tMark,
                vector<Contur>& squares, Trafo& tA, Trafo& tB)
{
  Image localSegImage;
  localSegImage.create(tMark);
  LocalSeg(tGray, localSegImage, 15, 25);

  IPoint start(0, 0);
  squares.clear();
  vector<double> areas;
  vector<Contur> conturs;

  while (SearchStart(localSegImage, tMark, LocalSegObj, 123, 5, start) == OK)
    {
      Contur c = CalcContur(localSegImage, tMark, LocalSegObj, 123, start);

      if (c.isValid())
        {
          if (c.isClosed())                  // geschlossene Konturen füllen
            {
              if (!c.isHole())
                {
                  FillRegion(c, 2, tMark);
                  double length, area, form, conv;
                  FeatureContur(c, length, area, form, conv);
                  if (area > 50 && form < 6)
                    {
                      //Printf("length: %lf  area: %lf  form: %lf  conv: %lf\n",
                      //     length, area, form, conv);
                      if (form < 1.50 && conv < 1.05)
                        {
                          FillRegion(c, 3, tMark); // quadrat
                          squares.push_back(c);
                          areas.push_back(area);
                        }
                      else if (form > 4 && form < 5)
                        {
                          FillRegion(c, 5, tMark);
                          conturs.push_back(c);
                        }
                      //          GetChar();
                    }
                }
            }
          MarkContur(c, 2, tMark);
        }
      else
        tMark.setPixel(start, 2);
    }
  // analysis of squares
  int nSquares = squares.size();
  if (nSquares < 24)
    throw "No / not enough squares found";
  if (verbose)
    cout << "squares: " << nSquares << endl;
  sort(areas.begin(), areas.end());
  double sqSize = areas[nSquares / 2];
  if (verbose)
    {
      cout << "median size: " << sqSize << endl;
      cout << "candidates: " << conturs.size() << endl;
    }
  if (conturs.size() < 2)
    throw "Less than two polygons found";

  //  ObjectMatcher omA(TRM_PROJECTIVE_NOR);
  ObjectMatcher omA(TRM_AFFINE_NOR);
  omA.setObject(0, makePointList(xyA, xPosA, yPosA));
  omA.interpolObject(0, 0.1);

  //  ObjectMatcher omB(TRM_PROJECTIVE_NOR);
  ObjectMatcher omB(TRM_AFFINE_NOR);
  omB.setObject(0, makePointList(xyB, xPosB - 6, yPosB));
  omB.interpolObject(0, 0.1);

  double minDistA = numeric_limits<double>::max();
  double minDistB = numeric_limits<double>::max();
  Contur A;
  Contur B;
  for (auto& c : conturs)
    {
      double length, area, form, conv;
      FeatureContur(c, length, area, form, conv);
      if (area > sqSize && area < sqSize * 2)
        {
          omA.setObject(1, c);
          omA.interpolObject(1, 0.1);
          double da = omA.getEvaluation();
          if (da < minDistA)
            {
              minDistA = da;
              A = c;
            }
          omB.setObject(1, c);
          omB.interpolObject(1, 0.1);
          double db = omB.getEvaluation();
          if (db < minDistB)
            {
              minDistB = db;
              B = c;
            }
        }
    }
  FillRegion(A, 1, tMark);
  FillRegion(B, 5, tMark);
  // reinitialize omA with Contur A
  omA.setObject(1, A);
  omA.interpolObject(1, 0.1);
  // reinitialize omB with Contur B
  omB.setObject(1, B);
  omB.interpolObject(1, 0.1);

  tA = omA.getTrafo();
  //  cout << trA.getMatrix() << endl;

  //  drawPointLists(tMark,omA);
  //  GetChar();

  tB = omB.getTrafo();
  //  cout << trB.getMatrix() << endl;

  //  drawPointLists(tMark,omB);
  //  GetChar();

  for (int x = 2; x < 6; x++)
    for (int y = 0; y < 6; y++)
      {
        Point p((x + 0.5) * rasterMM, (y + 0.5) * rasterMM);
        Point pp;
        transform(tA, p, pp);
        //        cout << p << " -> " << pp << endl;
        Marker(1, pp, 1, 7, tMark);
      }

  for (int x = 0; x < 2; x++)
    for (int y = 0; y < 6; y++)
      {
        Point p((x + 0.5) * rasterMM, (y + 0.5) * rasterMM);
        Point pp;
        transform(tB, p, pp);
        //        cout << p << " -> " << pp << endl;
        Marker(1, pp, 5, 7, tMark);
      }
}
