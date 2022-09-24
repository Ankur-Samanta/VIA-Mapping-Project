#include "loadmap_config.h"
#include <cmath>
std::vector<std::vector<StreetSegmentIdx>> intersection_street_segments;

std::vector<std::set<IntersectionIdx>> street_intersections;
std::vector<std::vector<IntersectionIdx>> street_intersections_vec;

std::vector<double> feature_collection; // freed

std::vector<double> streetLengthRegistry;
std::vector<double> streetSegmentTravelTimeRegistry;
std::unordered_map<OSMID, const OSMNode*> OSM_Hash;// freed
std::vector<std::vector<std::string>> intersection_street_names;// freed
std::vector<std::set<StreetIdx>> intersection_street_ids;// freed
std::vector<std::vector<std::vector<IntersectionIdx>>> adjacent_matrix2 ;

double calculateArea(LatLon P1, LatLon P2) {
    double longitudeP1 = P1.longitude() * kDegreeToRadian;
    double latitudeP1 = P1.latitude() * kDegreeToRadian;
    double xP1 = kEarthRadiusInMeters * longitudeP1 * cos(latitudeP1);
    double yP1 = kEarthRadiusInMeters * latitudeP1;

    double longitudeP2 = P2.longitude() * kDegreeToRadian;
    double latitudeP2 = P2.latitude() * kDegreeToRadian;
    double xP2 = kEarthRadiusInMeters * longitudeP2 * cos(latitudeP2);
    double yP2 = kEarthRadiusInMeters * latitudeP2;

    return ((xP2 * yP1) - (yP2 * xP1));
}

void loadStreetLengthRegistry() {
    int numstreetsegs = getNumStreetSegments();
    int numstreet = getNumStreets();
    streetLengthRegistry.resize(numstreet);
    
    for (StreetSegmentIdx i = 0; i<numstreet; i++){
        streetLengthRegistry[i] =0; 
    }
    //for loop to add each segment of the street to calculate the total length
    for (StreetSegmentIdx i = 0; i<numstreetsegs; i++){
        StreetSegmentInfo InfoS = getStreetSegmentInfo(i);
        StreetIdx SID = InfoS.streetID ;
        streetLengthRegistry[SID] += findStreetSegmentLength(i); 
    }
}

void load_intersection_street_segments()
{
    int numIntersections = getNumIntersections();
    int numStreet = getNumStreets();
    // extend the vector size to the number of intersections
    intersection_street_segments.resize(numIntersections);
    intersection_street_names.resize(numIntersections);
    intersection_street_ids.resize(numIntersections);
    street_intersections.resize(numStreet);

    for (int intersection = 0; intersection < numIntersections; ++intersection){
        int numSegsAtIntersection = getNumIntersectionStreetSegment(intersection);
        for (int street_seg = 0; street_seg < numSegsAtIntersection; ++street_seg){
            StreetSegmentIdx ss_id = getIntersectionStreetSegment(street_seg, intersection);
            StreetIdx s_id = getStreetSegmentInfo(ss_id).streetID;
            std::string s_name = getStreetName(s_id);

            intersection_street_segments[intersection].push_back(ss_id);
            intersection_street_names[intersection].push_back(s_name);
            intersection_street_ids[intersection].insert(s_id);

            street_intersections[s_id].insert(intersection);
        }
    }
}

void close_intersection_street_segments()
{
    for (int intersection = 0; intersection < intersection_street_segments.size(); ++intersection){
        int numSegsAtIntersection = intersection_street_segments[intersection].size();

        for (int street_seg = 0; street_seg < numSegsAtIntersection; ++street_seg){
            intersection_street_ids[intersection].clear();
            intersection_street_names[intersection].clear();
        }
    }
    intersection_street_names.clear();
    intersection_street_ids.clear();
}

void load_street_intersections_vec(){

    int NumStreet = getNumStreets();
    street_intersections_vec.resize(NumStreet);
    for(int i = 0; i < NumStreet; i++){
        std::vector<IntersectionIdx> temp(street_intersections[i].begin(), street_intersections[i].end());
        street_intersections_vec[i] = temp;
    }
}

void close_street_intersections(){

    for (int street = 0; street < street_intersections.size(); ++street){
        street_intersections[street].clear();
    }
    street_intersections.clear();
}

void load_feature(){
    int numFeature = getNumFeatures();

    feature_collection.resize(numFeature);

    for(int i=0; i< numFeature; i++){

        int numFeaturePoints = getNumFeaturePoints(i);
        if (numFeaturePoints<=2)
            feature_collection[i] = 0.0;

        else{
            LatLon FirstFeaturePoint = getFeaturePoint(0, i);
            LatLon LastFeaturePoint = getFeaturePoint(numFeaturePoints-1, i);
            if(FirstFeaturePoint==LastFeaturePoint){   //a close polygon
                double area = calculateArea(FirstFeaturePoint, LastFeaturePoint);
                for(int j = 0; j < numFeaturePoints-1; j++){
                    //new code added here
                    LatLon FeaturePoint1 = getFeaturePoint(j, i);
                    LatLon FeaturePoint2 = getFeaturePoint(j + 1, i);
                    area += calculateArea(FeaturePoint1, FeaturePoint2);
                }
                if(area / 2.0 < 0) {feature_collection[i] = ((-1) * area / 2.0);}
                else {feature_collection[i] = (area / 2.0);}
            }else{
                feature_collection[i] = 0.0;
            }
        }
    }
}

void close_feature(){

    feature_collection.clear();
}

void load_OSM_nodes(){
    int numOSMNode = getNumberOfNodes();
    for(int i=0; i<numOSMNode;i++){
        const OSMNode* OSM_node = getNodeByIndex(i);
        const OSMID id = OSM_node->id();
        OSM_Hash.insert(std::make_pair(id,OSM_node));
    }
}
