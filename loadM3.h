#ifndef LOADM3_H
#define LOADM3_H
#include "StreetsDatabaseAPI.h"

#include <iostream>
//#include <list>
#include <queue>
#include <map>


#define NO_EDGE -1 //Illegal edge ID -> no edge


struct travelInfo {
    StreetIdx streetID;
    double travelTime;
    bool highlight;

};

struct WaveElem{
    double actualTime;
    IntersectionIdx curNode;
    StreetSegmentIdx prevPath;
    
    WaveElem(double actualt, IntersectionIdx curn, StreetSegmentIdx prevp)
    {
        actualTime = actualt;
        curNode=curn;
        prevPath = prevp;
    }
};

struct Node{
    double time;
    //double ht;
    StreetSegmentIdx prevPath;
    IntersectionIdx prevNode;
    Node(double timep, StreetSegmentIdx prevp, IntersectionIdx prevn){
        time = timep;
        prevPath = prevp;
        prevNode = prevn;
    }
};

struct CompareTime {
    bool operator()(WaveElem const& w1, WaveElem const& w2)
    {
        return w1.actualTime > w2.actualTime;
    }
};

void loadtime();
void closetime();
bool checkWindow(IntersectionIdx intersection, double minLat, double maxLat, double minLon, double maxLon);
bool findPath(StreetSegmentIdx start, StreetSegmentIdx dest);

extern std::vector<travelInfo> streetSegVec;
extern std::vector<Node> intersectionVec;
extern std::vector<travelInfo> streetSegVec;
extern std::vector<Node> intersectionVec;
extern double globalSpeedlim;

#endif /* LOADM3_H */

