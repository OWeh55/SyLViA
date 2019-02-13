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
  if (debug & 1)
    Show(OVERLAY, localSegImage);
  LocalSeg(tGray, localSegImage, localSegSize, localSegLevel);

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
                  if (debug & 6)
                    Printf("length: %lf  area: %lf  form: %lf  conv: %lf\n",
                           length, area, form, conv);
                  if (area > 40 && form < 6)
                    {
                      if (form < 1.50 && conv < 1.05)
                        {
                          FillRegion(c, 3, tMark); // quadrat
                          squares.push_back(c);
                          areas.push_back(area);
                        }
                      else if (form > 4 && form < 5.5)
                        {
                          FillRegion(c, 5, tMark);
                          conturs.push_back(c);
                        }
                      if (debug & 4)
                        GetChar();
                    }
                }
              if (debug & 2)
                GetChar();
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

  // filter square conturs by size
  // create new marker image for hole search
  {
    tMark.set(3);
    vector<Contur> cSquares;
    for (Contur& c : squares)
      {
        double length, area, form, conv;
        FeatureContur(c, length, area, form, conv);
        if (area > 0.7 * sqSize && area < 1.5 * sqSize)
          {
            cSquares.push_back(c);
            FillRegion(c, 0, tMark);
          }
      }
    //  squares=cSquares;
  }

  // search for holes in squares
  squares.clear();
  areas.clear();
  start = IPoint(0, 0);
  while (SearchStart(localSegImage, tMark, LocalSegObj, 123, 5, start) == OK)
    {
      Contur c = CalcContur(localSegImage, tMark, LocalSegObj, 123, start);
      if (c.isValid())
        {
          if (c.isClosed())
            {
              if (c.isHole())
                {
                  // new square
                  double length, area, form, conv;
                  FeatureContur(c, length, area, form, conv);
                  squares.push_back(c);
                  areas.push_back(-area);
                  FillRegion(c, 4, tMark);
                }
            }
          MarkContur(c, 2, tMark);
        }
      else
        tMark.setPixel(start, 2);
    }

  sort(areas.begin(), areas.end());
  sqSize = areas[nSquares / 2];

  nSquares = squares.size();
  if (verbose)
    {
      cout << "reduced squares by size: " << nSquares << endl;
      cout << "median size of squares: " << sqSize << endl;
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
  bool hasA = false;
  bool hasB = false;
  for (auto& c : conturs)
    {
      double length, area, form, conv;
      FeatureContur(c, length, area, form, conv);
      // cout << area << " ? " << sqSize << endl;
      if (area > sqSize / 2 && area < sqSize * 2)
        {
          omA.setObject(1, c);
          omA.interpolObject(1, 0.1);
          double da = omA.getEvaluation();
          if (da < minDistA)
            {
              minDistA = da;
              A = c;
              hasA = true;
            }
          omB.setObject(1, c);
          omB.interpolObject(1, 0.1);
          double db = omB.getEvaluation();
          if (db < minDistB)
            {
              minDistB = db;
              B = c;
              hasB = true;
            }
        }
    }
  if (A.Start() == B.Start())
    throw IceException("findMarker", "only one polygon found");
  if (!hasA || !hasB)
    throw IceException("findMarker", "polygons not found");
  // clear all green markes
  WindowWalker w(tMark);
  for (w.init(); !w.ready(); w.next())
    if (tMark.getPixel(w) == 2)
      tMark.setPixel(w, 0);
  FillRegion(A, 1, tMark);
  FillRegion(B, 1, tMark);
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
  if (debug & 1)
    {
      GetChar();
      Show(OFF, localSegImage);
    }
}
