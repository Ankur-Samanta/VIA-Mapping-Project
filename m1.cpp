/* 
 * Copyright 2022 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <set>
#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "m1Helper/loadmap_config.h" //see relative load/close map functions in partial_name.cpp/h
//#include "m1Helper/partial_name.h" //see relative partial name functions in partial_name.cpp/h
#include "m2Helper/tileMapConfig.h"
#include "m2Helper/intersectionStreetFunc.h"
#include "m2Helper/featureFunc.h"
#include "m2Helper/POIFunc.h"
#include "m3Helper/loadM3.h"

bool mapLoaded;
ezgl::renderer *rendererP;

bool loadMap(std::string map_streets_database_filename) {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    
    std::cout << "load map successfully called"<< std::endl;
    std::string OSM_name = map_streets_database_filename;
    OSM_name.resize(map_streets_database_filename.size() - 11);
    OSM_name.append("osm.bin");
    if(!loadOSMDatabaseBIN(OSM_name) || !loadStreetsDatabaseBIN(map_streets_database_filename)){
        
        return false;
    }

    findMapSize(); // Here find min/max/avelat for map loaded
    std::cout << "map size is successfully calculated" << std::endl;
    std::cout << "xmin is: "<<minX << std::endl;
    std::cout << "xmax is: "<<maxX << std::endl;
    std::cout << "ymin is: "<<minY << std::endl;
    std::cout << "ymax is: "<<maxY << std::endl;
        
    
    std::cout << "entering SDB bin loading" << std::endl;
    
    // put load functions here
    loadStreetSegmentTravelTimeRegistry();
    loadIntersectionStreetMap();
    loadPOIMap();
    loadFeatureMap();
    loadtime();
    rendererP = nullptr;
    
    std::cout << "Successfully loaded map '" << map_streets_database_filename << "'\n";

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "load map time: " << elapsed_seconds.count() << "s\n\n";
    mapLoaded = true;
    
    return true;
}


void closeMap() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
     
    //Clean-up your map related data structures here
    clearOSMHash();
    clearIntersectionStreets();
    clearPOI();
    closeOSMDatabase();
    closeStreetDatabase();
    closetime();
//    std::cout<<"test this"<<std::endl;
    
    if(rendererP != nullptr)
        clearIcon();
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "clear map time: " << elapsed_seconds.count() << "s\n";
}

// Returns the geographically nearest intersection (i.e. as the crow flies) to the given position
IntersectionIdx findClosestIntersection(LatLon my_position){
    
    /* if later speed is found to be too low, use AVL or nested binary tree 
     in loadMap to improve performance,  */
    
    int min_dis = 0;
    IntersectionIdx closest_int = -1;
    
    for (int int_idx = 0; int_idx < getNumIntersections(); int_idx++){
        LatLon int_pos = getIntersectionPosition(int_idx);
        std::pair<LatLon, LatLon> int_comp (my_position, int_pos);
        double abs_dis = findDistanceBetweenTwoPoints(int_comp);
        
        if (abs_dis - min_dis <= 0 || min_dis == 0){
            min_dis = abs_dis;
            closest_int = int_idx;
        } 
    }
    return closest_int;
}


// Returns all intersections along a given street.
// There should be no duplicate intersections in the returned vector.
std::vector<IntersectionIdx> findIntersectionsOfStreet(StreetIdx street_id){
    // this is O(N), but could be less if load map fcn uses vector as nested
    // storing algorithms, N is the number of intersections
    return street_intersections_vec[street_id];
}
// Return all intersection ids at which the two given streets intersect
// This function will typically return one intersection id for streets
// that intersect and a length 0 vector for streets that do not. For unusual 
// curved streets it is possible to have more than one intersection at which 
// two streets cross.
// There should be no duplicate intersections in the returned vector.
std::vector<IntersectionIdx> findIntersectionsOfTwoStreets(std::pair<StreetIdx, StreetIdx> street_ids){
   
    std::vector<IntersectionIdx> list1=findIntersectionsOfStreet(street_ids.first);
    std::vector<IntersectionIdx> list2=findIntersectionsOfStreet(street_ids.second);
   
    std::vector<IntersectionIdx> result;
    std::set<StreetIdx> streetIds;
    if (list1.size()<list2.size()){
        for (auto const& inter: list1){
            streetIds = intersection_street_ids[inter];
            if (streetIds.find(street_ids.second)!=streetIds.end()){
                    result.push_back(inter);
            }
        }
    }else{
        for (auto const& inter: list2){
            streetIds = intersection_street_ids[inter];
            if (streetIds.find(street_ids.first)!=streetIds.end()){
                    result.push_back(inter);
            }

        }
    }
    return result;
    //return adjacent_matrix2[street_ids.first][street_ids.second];
}


bool intersectionsAreDirectlyConnected(std::pair<IntersectionIdx, IntersectionIdx> intersection_ids)
{   // // drive from the first intersection to the second using only one streetSegment.//two justifications: one is directly, another one is legal
    IntersectionIdx FirstNode = intersection_ids.first;
    IntersectionIdx SecondNode = intersection_ids.second;
    std::vector<StreetSegmentIdx> segments = findStreetSegmentsOfIntersection(FirstNode);
    
    if (SecondNode==FirstNode) return true;
    for (int i=0 ; i< segments.size(); ++i)
    {
        StreetSegmentInfo streetSInfo = getStreetSegmentInfo(segments[i]);
        if(SecondNode == streetSInfo.to)   //if from == second
        {
          return true;
        }
        else if(SecondNode == streetSInfo.from){
             // is directly connected
            if (streetSInfo.oneWay)   // if one way is true, only second to first
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    } 
    return false;
}


std::vector<StreetSegmentIdx> findStreetSegmentsOfIntersection(IntersectionIdx intersection_id)
{
    return intersection_street_segments[intersection_id];
} 


std::vector<std::string>findStreetNamesOfIntersection(IntersectionIdx intersection_id)
{
    return intersection_street_names[intersection_id];
    
}


double findStreetLength(StreetIdx street_id)
{
    return streetLengthRegistry[street_id];
}


POIIdx findClosestPOI(LatLon my_position, std::string POItype)
{
    int distance = 0; //initialize a distance for check the first two points
    POIIdx closest = -1;
    for (int i=0; i<getNumPointsOfInterest();++i)
    {
        if(POItype==getPOIType(i))  // matches type
        {
            std::pair<LatLon, LatLon> points (my_position, getPOIPosition(i));
            double distanceBetweenPoints = 0;  //initialize it to be 0
            distanceBetweenPoints=findDistanceBetweenTwoPoints(points);
            if(distanceBetweenPoints<distance || distance == 0)   //distance ==0 works for case i==0
            {
                distance=distanceBetweenPoints;
                closest=i;
            }              
        }
    }
    return closest;  
}


double findFeatureArea(FeatureIdx feature_id)
{
    return feature_collection[feature_id];
}


// Returns the distance between two (latitude,longitude) coordinates in meters
double findDistanceBetweenTwoPoints(std::pair<LatLon, LatLon> points) {

    LatLon FirstPoint = points.first;
    LatLon SecondPoint = points.second;
    double iFirstLat = FirstPoint.latitude();
    double iFirstLong = FirstPoint.longitude();
    double iSecondLat = SecondPoint.latitude();
    double iSecondLong = SecondPoint.longitude();
    
    double lat_avg = (iFirstLat+iSecondLat)/2.0 * kDegreeToRadian;
    double cos_coe = cos(lat_avg);
    
    double iFirstX = iFirstLong * cos_coe;
    double iFirstY = iFirstLat ;
    double iSecondX = iSecondLong * cos_coe;
    double iSecondY = iSecondLat ;
    
    return kEarthRadiusInMeters*kDegreeToRadian*sqrt(pow(iSecondY-iFirstY,2) + pow(iSecondX - iFirstX,2));
//    return distance;
}


// Returns the length of the given street segment in meters
double findStreetSegmentLength(StreetSegmentIdx street_segment_id) {
    StreetSegmentInfo seg_info = getStreetSegmentInfo(street_segment_id);
    IntersectionIdx iA = seg_info.from;
    IntersectionIdx iB = seg_info.to;
    int numCurvePoint = seg_info.numCurvePoints;
    double distance = 0;
    
    LatLon iALL = getIntersectionPosition(iA);
    LatLon iBLL = getIntersectionPosition(iB);
    
    if (numCurvePoint == 0) {
        distance += findDistanceBetweenTwoPoints(std::make_pair(iALL, iBLL));
    }
    else {
        // from 1st intersection to the first curve point
        LatLon iPrevPos = iALL;
        LatLon iPos;
        
        for (int i = 0; i <= numCurvePoint - 1; ++i){
            iPos = getStreetSegmentCurvePoint(i, street_segment_id);
            distance += findDistanceBetweenTwoPoints(std::make_pair(iPrevPos, iPos));
            iPrevPos = iPos;
        }
        distance += findDistanceBetweenTwoPoints(std::make_pair(iPrevPos, iBLL));
    }
    return distance;
}


void loadStreetSegmentTravelTimeRegistry() {
    int numstreetseg = getNumStreetSegments();
    streetSegmentTravelTimeRegistry.resize(numstreetseg);
    for (StreetSegmentIdx i = 0; i < numstreetseg; i++) {
        double distance = findStreetSegmentLength(i);
        float speed_limit = getStreetSegmentInfo(i).speedLimit;
        double time = distance / speed_limit;
        streetSegmentTravelTimeRegistry[i] = time;
    }
}


// Returns the travel time to drive from one end of a street segment to the other, in seconds, when driving at the speed limit
double findStreetSegmentTravelTime(StreetSegmentIdx street_segment_id) {
    return streetSegmentTravelTimeRegistry[street_segment_id];
}

//NOTE:TEMPORARY PLACEHOLDER FUNCTION UNTIL ABOVE FUNCTION  IS FIXED!!!!!!!! DELETE WHEN PARTIAL FUNCTION IS FIXED
std::vector<StreetIdx> findStreetIdsFromPartialStreetName(std::string street_prefix) {
    street_prefix.erase(std::remove(street_prefix.begin(), street_prefix.end(), ' '), street_prefix.end());
    std::transform(street_prefix.begin(), street_prefix.end(), street_prefix.begin(), ::tolower);
    if (street_prefix.empty() == true){
        return std::vector<int>();
    }
    
    std::vector<int> streedIds;
    std::vector<int> res;
    int lower = findLowerBound(streetNameIdMap, street_prefix);
    int upper = findUpperBound(streetNameIdMap, street_prefix);
    if (lower == -1){
        return std::vector<int>();
    }
    for(int i = lower; i <= upper; i++){
        //std::cout << std::next(mmap.begin(), i) -> first << std::endl;
        res.push_back(std::next(streetNameIdMap.begin(), i)->second);
    }
    return res;
}




//// Return the LatLon of an OSM node; will only be called with OSM nodes (not ways or relations)
LatLon findLatLonOfOSMNode (OSMID OSMid) {
    const OSMNode* temp = OSM_Hash.find(OSMid)->second;
    return getNodeCoords(temp);
}
