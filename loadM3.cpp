#include <vector>
#include "loadM3.h"
#include <queue>
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include <cmath>
#include "tileMapConfig.h"

std::vector<travelInfo> streetSegVec;
std::vector<Node> intersectionVec;
//initial global definition of the speedlim being referenced
double globalSpeedlim = 0.0;

//function to load data structures needed for path finding
void loadtime(){
    int numstreetseg = getNumStreetSegments();
    streetSegVec.resize(numstreetseg);
    for(int streetseg = 0; streetseg < numstreetseg; streetseg++){
        StreetSegmentInfo segmentInfo = getStreetSegmentInfo(streetseg);
        //adjust globalspeedlim value to be used later in heuristic calculation
        double speedlim = segmentInfo.speedLimit;
        if(speedlim>globalSpeedlim){
            globalSpeedlim = speedlim;
        }
        streetSegVec[streetseg].streetID = segmentInfo.streetID;
        streetSegVec[streetseg].travelTime = findStreetSegmentTravelTime(streetseg);
        streetSegVec[streetseg].highlight = false;
    }
}

//utility function to check whether an intersection is relevant to our search, as in within the viewing window
bool checkWindow(IntersectionIdx intersection, double minx, double maxx, double miny, double maxy) {
    LatLon position = getIntersectionPosition(intersection);
    double lat = position.latitude() *kDegreeToRadian;
    double lon = position.longitude() * kDegreeToRadian;
    double x = kEarthRadiusInMeters * lon * cos(lat);
    double y = kEarthRadiusInMeters * lat;
    //returns true if x and y are in the current viewing window
    return (x >= minx) && (x < maxx) && (y >= miny) && (y < maxy);
}

void closetime(){
    //clear the streetSegVec data structure
    streetSegVec.clear();
}