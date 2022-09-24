#include "POIFunc.h"
#include "tileMapConfig.h"
#include "controlLayer.h"

std::vector<std::vector<std::vector<POIData>>> POIMap;
std::vector<POIgraphicInfo> POIInfoArray;
std::vector<std::vector<POIIdx>> POICategoryArr;

std::unordered_map<std::string, int> food = {
    
        {"bar",0},// type, iconidx
        {"biergarten",2},
        {"cafe",1},
        {"fast_food",3},
        {"food_court",4},
        {"ice_cream",5},
        {"pub",6},
        {"restaurant",7}
        
    };

std::unordered_map<std::string, int> entertainment = {
        {"arts_centre",15},
        {"brothel",49}, //no specific assigned icon
        {"casino",16},
        {"cinema",17},
        {"community_centre",18},
        {"conference_centre",49}, //no specific assigned icon
        {"events_venue",49}, //no specific assigned icon
        {"fountain",19},
        {"gambling",49}, //no specific assigned icon
        {"love_hotel",49}, //no specific assigned icon
        {"nightclub",20},
        {"planetarium",49},//no icon
        {"public_bookcase",21},
        {"social_centre",49}, //no icon
        {"stripclub",49}, //no icon
        {"studio",49}, //no icon
        {"swingerclub",49}, //no icon
        {"theatre",22}
    };

std::unordered_map<std::string, int> service = {
        {"courthouse",8},
        {"fire_station",9},
        {"police",10},
        {"post_box",11},
        {"post_depot",11},
        {"post_office",12},
        {"prison",13},
        {"ranger_station",49},  //no specific assigned icon
        {"townhall",14},
        {"atm",24},
        {"bank",25},
        {"bureau_de_change",26},
        {"baby_hatch", 30},
        {"clinic", 27},
        {"dentist", 28},
        {"doctors",29},
        {"hospital",30},
        {"nursing_home",32},
        {"pharmacy", 31},
        {"social_facility",32},
        {"veterinary", 33}
    };

std::unordered_map<std::string, int> education = {
        {"college",23},
        {"driving_school",23},
        {"kingdergarten",23},
        {"language_school",23},
        {"library",23},
        {"toy_library",23},
        {"music_school",23},
        {"school",23},
        {"university",23}
    };

std::unordered_map<std::string, int> transit = {
        {"bicycle_parking",34},
        {"bicycle_repair_station", 35},
        {"bicycle_rental",36},
        {"boat_rental", 37},
        {"boat_sharing", 37},
        {"bus_station", 38},
        {"car_rental", 39},
        {"car_sharing", 39},
        {"car_wash", 40},
        {"vehicle_inspection", 41},
        {"charging_station", 42},
        {"ferry_terminal", 43},
        {"fuel", 44},
        {"grit_bin", 49}, //no specific assigned icon
        {"motorcycle_parking", 45},
        {"parking", 46},
        {"parking_entrance", 46},
        {"parking_space", 47},
        {"taxi", 48}
    };
       
void loadPOIMap(){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    int numPOI = getNumPointsOfInterest();
    POIInfoArray.resize(numPOI);
    POICategoryArr.resize(6);

    POIMap.resize(yTileNum);
    for (int y_T = 0; y_T < yTileNum; y_T++){
        POIMap[y_T].resize(xTileNum);
    }
    
    for (int POI = 0; POI < numPOI; ++POI){
        
        POIInfoArray[POI].highlight = true;
//        POIInfoArray[POI].near_highlight = false;
        
        LatLon pos = getPOIPosition(POI);
        double y = LatttoY (pos.latitude());
        double x = LontoX (pos.longitude(), aveLat);
        
        std::string POItype = getPOIType(POI);
        
        // this could later be set into a unordered_map for efficiency improvement
        bool other = true;
        
        auto foodFind = food.find(POItype);
        auto entertainFind = entertainment.find(POItype);
        auto transitFind = transit.find(POItype);
        auto serviceFind = service.find(POItype);
        auto educationFind = education.find(POItype);
        if (foodFind!=food.end()){
            
            POIInfoArray[POI].iconIdx = (*foodFind).second;
            POICategoryArr[0].push_back(POI);
            other = false;
            
        } 
        else if (entertainFind!=entertainment.end()){
            
            POIInfoArray[POI].iconIdx = (*entertainFind).second;
            POICategoryArr[1].push_back(POI);
            other = false;
            
        } 
        else if (transitFind!=transit.end()){
            
            POIInfoArray[POI].iconIdx = (*transitFind).second;
            POICategoryArr[2].push_back(POI);
            other = false;
            
        } 
        else if (serviceFind!=service.end()){
            
            POIInfoArray[POI].iconIdx = (*serviceFind).second;
            POICategoryArr[3].push_back(POI);
            other = false;
            
        } 
        else if (educationFind!=education.end()){
            
            POIInfoArray[POI].iconIdx = (*educationFind).second;
            POICategoryArr[4].push_back(POI);
            other = false;
            
        } 
        
        POIInfoArray[POI].name = getPOIName(POI);
        
        int yTileIdx = (y-minY)/Tileh;
        int xTileIdx = (x-minX)/Tilew;
        
        if(!other) POIMap[yTileIdx][xTileIdx].push_back({POI, x ,y});
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "load POI time: " << elapsed_seconds.count() << "s\n";
}


void draw_POI (ezgl::renderer *g){
    // chrono runtime measurement - beginning
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    g->format_font("Open Sans", ezgl::font_slant::normal, 
            ezgl::font_weight::bold,13);
    
    std::vector<std::pair<int, int>> highlightList;
    if (zoomRatio <= POIShowLevel){
        for (int tileY = yTilemin; tileY<=yTilemax; tileY++){
            for (int tileX = xTilemin; tileX<=xTilemax; tileX++){
                for(int POI_idx = 0; POI_idx < POIMap[tileY][tileX].size(); POI_idx++){
                    // get id of the specific POI
                    POIIdx POI_id = POIMap[tileY][tileX][POI_idx].id; 

                    bool show = POIInfoArray[POI_id].highlight;
//                    bool nearhigh = POIInfoArray[POI_id].near_highlight;
//                    if(nearhigh) {
//                        double x_high = POIMap[tileY][tileX][POI_idx].xPos;
//                        double y_high = POIMap[tileY][tileX][POI_idx].yPos;
//                        highlightList.push_back(std::make_pair(x_high,y_high));
//                    }
                    if(show){
                        double x = POIMap[tileY][tileX][POI_idx].xPos;
                        double y = POIMap[tileY][tileX][POI_idx].yPos;
                        // for later implementation of draw text
                        std::string name = POIInfoArray[POI_id].name;
                        
                        int iconI = POIInfoArray[POI_id].iconIdx;

                        g->draw_surface(POIIcon[iconI],{x,y});
//                        std::cout<<name<<std::endl;
                        
                        if (zoomPropotion <= 0.06){
                            // hard coded for efficiency purposes
                            double factor = std::max(zoomPropotion,0.004);
                            float alpha = (0.6-factor)/(0.6-0.01);
                            if(lmode){
                                g->set_color(10,10,10, 220*alpha);
                            }else{
                                g->set_color(220,228,242, 220*pow(alpha,2));
                            }
                            g->draw_text({x,y-4}, name);
                            

                        }
                        
                    }

                }
            }
        }
    }
    // draw highlighted intersection on the very top 
//    double x;
//    double y;
//    for (int inters = 0; inters < highlightList.size(); inters++){
//        x = highlightList[inters].first;
//        y = highlightList[inters].second;   
//        g->draw_surface(POI_icon,{x,y});
//
//    }

    // chrono runtime measurement - ending
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "POI drawing time: " << elapsed_seconds.count() << "s\n";

}

void clearPOI(){
    POIMap.clear();
    POIInfoArray.clear();
    POICategoryArr.clear();
}