#ifndef LOADFEATUREDATA_H
#define LOADFEATUREDATA_H

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
#include "ezgl/point.hpp"
#include "controlLayer.h"

// Level of details as well as color scheme for features
constexpr double parkRatio = 50;
static constexpr ezgl::color parkCL (0xCE, 0xEA, 0xD6);
static constexpr ezgl::color parkCD (0x16, 0x40, 0x42); 

constexpr double beachRatio = 100;
static constexpr ezgl::color beachCL (0xFE, 0xEF, 0xC3); 
static constexpr ezgl::color beachCD (0x1B, 0x25, 0x35); 

constexpr double lakeRatio = 50;
//static constexpr ezgl::color lakeCL (0xFF, 0x00, 0x04); 
static constexpr ezgl::color lakeCL (0x9C, 0xC0, 0xF9); 
static constexpr ezgl::color lakeCD (0x07, 0x08, 0x0b); 

constexpr double riverRatio = 100;
//static constexpr ezgl::color riverCL (0xFF, 0x00, 0x04); 
static constexpr ezgl::color riverCL (0x9C, 0xC0, 0xF9); 
static constexpr ezgl::color riverCD (0x07, 0x08, 0x0b);

//constexpr double islandRatio = 0;
static constexpr ezgl::color islandCL (0xE8, 0xEA, 0xED);
static constexpr ezgl::color islandCD (0x1B, 0x25, 0x35);

constexpr double buildingRatio = 10;
static constexpr ezgl::color buildingCL (0xD5, 0xD8, 0xDB); 
static constexpr ezgl::color buildingCD (0x3C, 0x4D, 0x61);

constexpr double greenspaceRatio = 100;
static constexpr ezgl::color greenspaceCL (0xB2, 0xCF, 0xBD);
static constexpr ezgl::color greenspaceCD (0x10, 0x2B, 0x32);

constexpr double golfRatio = 50;
static constexpr ezgl::color golfCL (0xCE, 0xEA, 0xD6);
static constexpr ezgl::color golfCD (0x1D, 0x4D, 0x51);

constexpr double streamRatio = 100;
static constexpr ezgl::color streamCL (0x9C, 0xC0, 0xF9); 
static constexpr ezgl::color streamCD (0x07, 0x08, 0x0b); 

constexpr double glacierRatio = 2000;
static constexpr ezgl::color glacierCL (0xFF, 0xFF, 0xFF); 
static constexpr ezgl::color glacierCD (0x48, 0x58, 0x75); 

void draw_features (ezgl::renderer *g);
void loadFeatureMap();

struct featureData {
    std::vector<ezgl::point2d> points;
    int drawType; //0->point, 1 -> node segment, 2 -> polygon
    FeatureType featureType;
    FeatureIdx id;
    bool operator==(const featureData& feature) const{
        return (this->id == feature.id);
    }
    size_t operator()(const featureData& feature) const { 
        return feature.id; 
    }
    
    ~featureData(){
        points.clear();
    }
    
};

struct hash_function {
    std::size_t operator() (const featureData& node) const{
        return node.id;
    }
};

extern std::vector<std::string> featureNames;
extern std::vector<std::vector<std::vector<featureData>>> featureDataMap;
extern std::vector<featureData> bigFeatures;
extern std::unordered_set<featureData,featureData> features;

constexpr double featureR = 50; // point features radius
constexpr double bigFeatureArea = 500000; // adjusted

void drawFeatureByCategory(ezgl::renderer *g, ezgl::color lightCol, 
            ezgl::color darkCol, std::vector<featureData> & featArr);
void clearFeatures();

#endif /* LOADPOIDATA_H */

