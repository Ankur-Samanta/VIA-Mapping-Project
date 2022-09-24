#ifndef LOADPOIDATA_H
#define LOADPOIDATA_H

#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <ctime>
#include <vector>
#include <map>
#include <set>

#include "OSMDatabaseAPI.h"
#include "StreetsDatabaseAPI.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"

constexpr double POIW = 25.0;
constexpr double POIH = 25.0;
constexpr double POIShowLevel = 0.0002;
static constexpr ezgl::color POIColor (0x07, 0x08, 0x0b); 
// Note that all POI are OSMNodes so they works similar to the intersections
struct POIData {
    POIIdx id;
    double xPos;
    double yPos;
};

struct POIgraphicInfo{
//    bool near_highlight;
    bool highlight;
//    std::string type;
    std::string name;
    int iconIdx;
    // add anything else here such as POI type/color/icons/stuffs that is required for
    // graphic rendering
};


extern std::vector<std::vector<std::vector<POIData>>> POIMap;
extern std::vector<POIgraphicInfo> POIInfoArray;
extern std::vector<std::vector<POIIdx>> POICategoryArr;

// #ofcategories of (#POI in this categories)
void draw_POI (ezgl::renderer *g);
void loadPOIMap();

void clearPOI();

#endif /* LOADPOIDATA_H */

