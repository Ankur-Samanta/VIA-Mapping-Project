#include "tileMapConfig.h"

double maxX;
double minX;
double maxY;
double minY;
double winWidth;
double winHeight;
double aveLat;
int xTileNum;
int yTileNum;
int xTilemin;
int yTilemin;
int xTilemax;
int yTilemax;
double zoomRatio;
//double zoomPropotion;
std::unordered_map<OSMID, const OSMNode *> OSMHashN;
std::unordered_map<OSMID, const OSMWay *> OSMHashW;

double diagonalNorm;
double timeNorm;
double minSpeed;

void findMapSize(){
    int numOSMNode = getNumberOfNodes();
    double max_lat = getNodeByIndex(0)->coords().latitude();
    double min_lat = max_lat;
    double max_lon = getNodeByIndex(0)->coords().longitude();
    double min_lon = max_lon;
    // This for loop traverse through all OSM nodes, so if any later details
    // are required on an OSMNode basis, add them here. 
    for (int id = 0; id < numOSMNode; ++id) {
        const OSMNode* node = getNodeByIndex(id);
        max_lat = std::max(max_lat, node->coords().latitude());
        min_lat = std::min(min_lat, node->coords().latitude());
        max_lon = std::max(max_lon, node->coords().longitude());
        min_lon = std::min(min_lon, node->coords().longitude());
        if(node->id().valid()){
        OSMHashN.insert(std::make_pair(node->id(), node));}
    }
    
    //find min max lat lon
    aveLat=(min_lat+max_lat)/2;  
    maxX = LontoX(max_lon, aveLat);
    minX = LontoX(min_lon, aveLat);
    maxY = LatttoY(max_lat);
    minY = LatttoY(min_lat);
    winWidth = maxX - minX;
    winHeight = maxY - minY;
    double xSize = maxX - minX;
    double ySize = maxY - minY;
    xTileNum = xSize/Tilew+1;
    yTileNum = ySize/Tileh+1;
    
    //NOTE:used for heuristic calculation for A*
    diagonalNorm = sqrt((maxX - minX)*(maxX - minX) + (maxY - minY)*(maxY - minY)); 
    timeNorm = diagonalNorm / minSpeed; //max possible time to traverse diagonal
    minSpeed = 16;
    
    // loading OSMWay OSMid
    int numOSMWay = getNumberOfWays();
    for(int way = 0; way < numOSMWay; way++){
        const OSMWay* OSM_way = getWayByIndex(way);
        if(OSM_way->id().valid()){
            OSMHashW.insert(std::make_pair(OSM_way->id(), OSM_way));
        }
    }
}

void clearOSMHash(){
    OSMHashN.clear();
    OSMHashW.clear();
}

double LatttoY (double lat) {
    return (lat * kEarthRadiusInMeterscon * kDegreeToRadiancon);
}

double LontoX (double longval, double lat_avg) {
    return (longval * kEarthRadiusInMeterscon * cos(lat_avg * kDegreeToRadiancon) * kDegreeToRadiancon);
}

