#ifndef MAPPER_LOADOSMDATA_H
#define MAPPER_LOADOSMDATA_H

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <map>
#include <set>
#include "OSMDatabaseAPI.h"
#include "StreetsDatabaseAPI.h"

constexpr double kEarthRadiusInMeterscon = 6372797.560856;
constexpr double kDegreeToRadiancon = 0.017453292519943295769236907684886;
constexpr double Tilew = 1000;
constexpr double Tileh = 1000;
constexpr double MinWinArea = 10000;
constexpr double MinZoomArea = 5000000;
constexpr double MaxZoomArea = 500000000;

extern double maxX;
extern double minX;
extern double maxY;
extern double minY;
extern double winWidth;
extern double winHeight;
extern double viewWidth;
extern double viewHeight;
extern double zoomPropotion;
extern double aveLat;
extern int xTileNum;
extern int yTileNum;
extern int xTilemin;
extern int yTilemin;
extern int xTilemax;
extern int yTilemax;
extern double zoomRatio;
extern std::unordered_map<OSMID, const OSMNode *> OSMHashN;
extern std::unordered_map<OSMID, const OSMWay *> OSMHashW;
extern double diagonalNorm;
extern double timeNorm;
extern double minSpeed;

void findMapSize();
void clearOSMHash();

double LatttoY (double lat);
double LontoX (double longval, double lat_avg);

extern bool mapLoaded;

#endif //MAPPER_LOADOSMDATA_H
