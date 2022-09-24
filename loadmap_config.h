#ifndef LOADMAP_CONFIG_H
#define LOADMAP_CONFIG_H

#include "m1.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
//#include "loadmap_config.h"
#include <iostream>
#include <map>
#include <vector>

#include <set>
/*functions and global variables used in M1 for map loading/closing*/

// =========================== Global variable declaration ===========
///* This global vector uses a nested structure to store street segments
//   categorized by street intersection nodes*/
//
extern std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;
//
///* This global vector uses a nested structure to store unduplicated street
//    intersections categorized by street ids*/
//
extern std::vector<std::set<IntersectionIdx>> street_intersections;
extern std::vector<std::vector<IntersectionIdx>> street_intersections_vec;
//
///* This global vector uses a nested vector structure to store features points by
// their feature ids*/
extern std::vector<double> feature_collection; // freed
//
///* This global vector contains a list of street IDs associated with a given prefix for the street name*/
//
//
//std::multimap<std::string, StreetIdx> mmp;
//std::vector<std::string> streetNamesSorted;
//
extern std::vector<double> streetLengthRegistry;
extern std::vector<double> streetSegmentTravelTimeRegistry;
//
extern std::unordered_map<OSMID, const OSMNode*> OSM_Hash;// freed
//
extern std::vector<std::vector<std::string>> intersection_street_names;// freed
extern std::vector<std::set<StreetIdx>> intersection_street_ids;// freed
//
extern std::vector<std::vector<std::vector<IntersectionIdx>>> adjacent_matrix2 ;

// =========================== helper functions used for map loading/closing ========
void loadStreetSegmentTravelTimeRegistry() ;

void load_intersection_street_segments();

void load_street_intersections_vec();

void load_feature();

void load_OSM_nodes();

void loadStreetLengthRegistry();

//used in closeMap function to store everything stored in the related global variables and function itself
void close_intersection_street_segments();

//used in closeMap function to store everything stored in the related global variables and function itself
void close_street_intersections();

//used in closeMap function to clear the information related with natural features, specifically the position of the feature point
void close_feature();

double calculateArea(LatLon P1, LatLon P2);


#endif