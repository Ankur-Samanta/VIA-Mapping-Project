#ifndef LOADINTERSECTIONDATA_H
#define LOADINTERSECTIONDATA_H

#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <ctime>
#include <vector>
#include <map>
#include <set>
#include <unordered_set>

#include "OSMDatabaseAPI.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/color.hpp"

constexpr double PI = 3.14159265358979323846;

constexpr double intersectionW = 25.0;
constexpr double intersectionH = 25.0;
static constexpr ezgl::color backgroundC(0xF1, 0xF3, 0xF4);

// graphic details (including color scheme)
constexpr double motorwayWidth = 7.0; //11
constexpr double motorwayWidthmin = 3.0;
constexpr double motorwayRatio = 1000000;
static constexpr ezgl::color motorColorL(0xF8, 0xD4, 0x71);
static constexpr ezgl::color motorColorD(0x54, 0x6E, 0x8F);

constexpr double truckWidth = 7.0; //11
constexpr double truckWidthmin = 3.0;
constexpr double truckRatio = 2500;
static constexpr ezgl::color truckColorL(0xFD, 0xE2, 0x93);
static constexpr ezgl::color truckColorD(0x54, 0x6E, 0x8F);

constexpr double primaryWidth = 7.0; //11
constexpr double primaryWidthmin = 3.0;
constexpr double primaryRatio = 700;
static constexpr ezgl::color primaryColorL(0xFD, 0xE2, 0x93);
static constexpr ezgl::color primaryColorD(0x54, 0x6E, 0x8F);

constexpr double secondaryWidth = 6.0; //9.0
constexpr double secondaryWidthmin = 2.0;

constexpr double secondaryRatio = 700;

static constexpr ezgl::color secondaryColorL(0xFF, 0xFF, 0xFF);
static constexpr ezgl::color secondaryColorD(0x28, 0x3D, 0x5C);

constexpr double tertiaryWidth = 5.0; //8.0
constexpr double tertiaryWidthmin = 1.0;
constexpr double tertiaryRatio = 50;
static constexpr ezgl::color tertiaryColorL(0xFF, 0xFF, 0xFF);
static constexpr ezgl::color tertiaryColorD(0x28, 0x3D, 0x5C);

constexpr double unclassifiedWidth = 5.0; //8
constexpr double unclassifiedWidthmin = 2.0;
constexpr double unclassifiedRatio = 20;
static constexpr ezgl::color unclassifiedColorL(0xFF, 0xFF, 0xFF);
static constexpr ezgl::color unclassifiedColorD(0x28, 0x3D, 0x5C);

constexpr double residentialWidth = 4.0; //7
constexpr double residentialWidthmin = 2.0;
constexpr double residentialRatio = 12;
static constexpr ezgl::color residentialColorL(0xFF, 0xFF, 0xFF);
static constexpr ezgl::color residentialColorD(0x28, 0x3D, 0x5C);

constexpr double otherWidth = 3.0; //8.5
constexpr double otherWidthmin = 1.5;
constexpr double otherRatio = 12;
static constexpr ezgl::color otherColorL(0xFF, 0xFF, 0xFF);
static constexpr ezgl::color otherColorD(0x3C, 0x4D, 0x61);

// intersection structure
struct intersectionData {
    IntersectionIdx id;
    double xPos;
    double yPos;
};

struct intersectionGraphicInfo{
    double x;
    double y;
    bool highlight;
    std::set<std::string> street_col;
    
    intersectionGraphicInfo(){
        highlight = false;
        street_col = std::set<std::string>();
    }
    
    ~intersectionGraphicInfo(){
        street_col.clear();
    }
};

// street structure
struct streetData{
    std::unordered_map<OSMID, std::unordered_map<OSMID,StreetSegmentIdx>> wayIDCollection;
    bool highlight;
    streetData(){
        highlight = false;
        wayIDCollection = std::unordered_map<OSMID, std::unordered_map<OSMID,StreetSegmentIdx>>();
    }
    
    ~streetData(){
        wayIDCollection.clear();
    }
};

struct textData{
    double x;
    double y;
    double angle;
    double r;
    double w;
};

//struct StreetSegData {
//
//    int numofCurveP;
//};

struct NodeSegData {
    StreetIdx id;
    StreetSegmentIdx segId;
    bool middle;
    bool OneWay;
    float speedLimit;
//    std::string detailLevel;
    std::string name;
    double angle;
    double xPos1;
    double yPos1;
    double xPos2;
    double yPos2;
};

struct simpleEdge{
    double x1;
    double y1;
    double x2;
    double y2;
};

// intersection loading structure
extern std::vector<std::vector<std::vector<intersectionData>>> intersectionMap;
extern std::vector<intersectionGraphicInfo> intersectionInfoArray;
extern intersectionData highlight_int;
extern std::vector<IntersectionIdx> findIntersections;

extern std::vector<std::set<std::pair<IntersectionIdx, StreetSegmentIdx>>> adjacency_list;
// street loading structure
//extern std::vector<StreetSegData> streetSegDataArray;
extern std::vector<streetData> streetDataArray;

// street loading by categories 
extern std::vector<std::vector<std::vector<std::vector<NodeSegData>>>> layerStreetDataMap; // 8 categories
extern std::vector<std::vector<simpleEdge>> streetsegEdge;

void draw_intersections (ezgl::renderer *g);
void draw_streets(ezgl::renderer *g);
void loadIntersectionStreetMap ();


// street intersect utilities
extern StreetIdx searchBarStreet1;
extern StreetIdx searchBarStreet2;
extern std::vector<IntersectionIdx>twoStreetsIntersections;
extern std::vector<std::unordered_set<IntersectionIdx>> intersectionsOfStreet;
extern std::map<std::string, StreetIdx> streetNameIdMap;

int findType(std::string category); // use to classify streets

int isPrefix(std::string street_name, std::string test_prefix);
int findLowerBound(std::map<std::string, StreetIdx> m, std::string street_prefix);
int findUpperBound(std::map<std::string, StreetIdx> m, std::string street_prefix);

void draw_level(ezgl::renderer *g, int level, int tileY, int tileX, 
        std::vector<NodeSegData> & highlightList, 
        std::unordered_map<std::string, textData> & mapmap);

void mapmapInsertName(ezgl::renderer *g, double zoomLevel, double widthN, 
        double widthMin, ezgl::color roadColor, double nameZoomLevel, double onewayzoom,
        NodeSegData& segData, std::unordered_map<std::string, textData> & mapmap);

std::string oneway_string (std::string nameSeg, bool oneway, double x1,
        double x2, double y1, double y2);
void clearIntersectionStreets();
#endif /* LOADINTERSECTIONDATA_H */

