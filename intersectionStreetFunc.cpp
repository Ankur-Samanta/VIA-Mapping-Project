#include "intersectionStreetFunc.h"
#include "tileMapConfig.h"
#include "controlLayer.h"
#include "loadM3.h"

std::vector<std::vector<std::vector<intersectionData>>> intersectionMap;
std::vector<intersectionGraphicInfo> intersectionInfoArray;
   
std::vector<streetData> streetDataArray;

//std::vector<std::vector<std::vector<NodeSegData>>> streetDataMap;
std::vector<std::vector<std::vector<std::vector<NodeSegData>>>> layerStreetDataMap;

std::map<std::string, StreetIdx> streetNameIdMap;
StreetIdx searchBarStreet1, searchBarStreet2;
std::vector<std::unordered_set<IntersectionIdx>> intersectionsOfStreet;
std::vector<IntersectionIdx>twoStreetsIntersections;

std::vector<std::set<std::pair<IntersectionIdx, StreetSegmentIdx>>> adjacency_list;
std::vector<std::vector<simpleEdge>> streetsegEdge;

std::vector<ezgl::color> lightcolor;
std::vector<ezgl::color> darkcolor;
std::vector<double> waywidth;
std::vector<double> waywidthmin;
std::vector<double> ratio;
std::vector<double> nameZoomLevel;

void loadIntersectionStreetMap(){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    // configure drawing parameters
    lightcolor.push_back(motorColorL);
    lightcolor.push_back(truckColorL);
    lightcolor.push_back(primaryColorL);
    lightcolor.push_back(secondaryColorL);
    lightcolor.push_back(tertiaryColorL);
    lightcolor.push_back(unclassifiedColorL);
    lightcolor.push_back(residentialColorL);
    lightcolor.push_back(otherColorL);
    
    darkcolor.push_back(motorColorD);
    darkcolor.push_back(truckColorD);
    darkcolor.push_back(primaryColorD);
    darkcolor.push_back(secondaryColorD);
    darkcolor.push_back(tertiaryColorD);
    darkcolor.push_back(unclassifiedColorD);
    darkcolor.push_back(residentialColorD);
    darkcolor.push_back(otherColorD);
    
    waywidth.push_back(motorwayWidth); 
    waywidth.push_back(truckWidth); 
    waywidth.push_back(primaryWidth); 
    waywidth.push_back(secondaryWidth); 
    waywidth.push_back(tertiaryWidth); 
    waywidth.push_back(unclassifiedWidth); 
    waywidth.push_back(residentialWidth); 
    waywidth.push_back(otherWidth); 
    
    waywidthmin.push_back(motorwayWidthmin); 
    waywidthmin.push_back(truckWidthmin); 
    waywidthmin.push_back(primaryWidthmin); 
    waywidthmin.push_back(secondaryWidthmin); 
    waywidthmin.push_back(tertiaryWidthmin); 
    waywidthmin.push_back(unclassifiedWidthmin); 
    waywidthmin.push_back(residentialWidthmin); 
    waywidthmin.push_back(otherWidthmin); 
    
    ratio.push_back(motorwayRatio);
    ratio.push_back(truckRatio);
    ratio.push_back(primaryRatio);
    ratio.push_back(secondaryRatio);
    ratio.push_back(tertiaryRatio);
    ratio.push_back(unclassifiedRatio);
    ratio.push_back(residentialRatio);
    ratio.push_back(otherRatio);
    
    nameZoomLevel.push_back(1800.0);
    nameZoomLevel.push_back(1500.0);
    nameZoomLevel.push_back(500.0);
    nameZoomLevel.push_back(25.0);
    nameZoomLevel.push_back(25.0);
    nameZoomLevel.push_back(2); //0.2
    nameZoomLevel.push_back(1.5); //0.2
    nameZoomLevel.push_back(1.5); //0.2
    
    // coding style 
    
    int numIntersections = getNumIntersections();
    int numStreets = getNumStreets();
    int numStreetSeg = getNumStreetSegments();
    
    streetsegEdge.resize(numStreetSeg);
    adjacency_list.resize(numIntersections);
    intersectionInfoArray.resize(numIntersections);
    
    streetDataArray.resize(numStreets);
    layerStreetDataMap.resize(8);
    
//    streetSegDataArray.resize(numStreetSeg);
    intersectionsOfStreet.resize(numStreets);
    intersectionMap.resize(yTileNum);
//    streetDataMap.resize(yTileNum);
    for(int type = 0; type < 8; type++){
            layerStreetDataMap[type].resize(yTileNum);
    }
    
    for (int y_T = 0; y_T < yTileNum; y_T++){
        intersectionMap[y_T].resize(xTileNum);
//        streetDataMap[y_T].resize(xTileNum);
        for(int type = 0; type < 8; type++){
            layerStreetDataMap[type][y_T].resize(xTileNum);
        }
    }
    
    for (int intersection = 0; intersection < numIntersections; ++intersection){
        LatLon pos = getIntersectionPosition(intersection);
        double y = LatttoY (pos.latitude());
        double x = LontoX (pos.longitude(), aveLat);
        
        int yTileIdx = (y-minY)/Tileh;
        int xTileIdx = (x-minX)/Tilew;
        // loading intersectioninfoarry/ tilemap
        intersectionInfoArray[intersection].highlight = false;
        intersectionInfoArray[intersection].x = x;
        intersectionInfoArray[intersection].y = y;
        
        intersectionMap[yTileIdx][xTileIdx].push_back({intersection, x ,y});
        
        // loading streetDataArray
        int segNum = getNumIntersectionStreetSegment(intersection);
        
        for (int segN = 0; segN < segNum; segN++){
            StreetSegmentIdx segIdx = getIntersectionStreetSegment(segN, 
                    intersection);
            StreetSegmentInfo info = getStreetSegmentInfo(segIdx);
            
            bool oneWay = info.oneWay;
            IntersectionIdx fromNode = info.from;
            IntersectionIdx toNode = info.to;
            
            // adding element for path-finding adjacency list
            adjacency_list[fromNode].insert(
                std::make_pair(toNode, segIdx));
            if(!oneWay){
                adjacency_list[toNode].insert(
                    std::make_pair(fromNode, segIdx));
            }
            
            OSMID startIntersectionId=getIntersectionOSMNodeID(info.from);
            auto InfoPair = streetDataArray[info.streetID].wayIDCollection.find(info.wayOSMID);
            
            if(InfoPair == streetDataArray[info.streetID].wayIDCollection.end()){
                streetDataArray[info.streetID].wayIDCollection[info.wayOSMID]=std::unordered_map<OSMID,StreetSegmentIdx>();                
            }
            
            streetDataArray[info.streetID].wayIDCollection[info.wayOSMID].insert(std::make_pair(startIntersectionId,segIdx));

            intersectionsOfStreet[info.streetID].insert(intersection);
            intersectionInfoArray[intersection].street_col.insert(getStreetName(info.streetID));
            
        }
    }
    
    const std::vector<OSMID> * nodeSet;
    const OSMWay* osmway;
    const OSMNode* osm_N1;
    const OSMNode* osm_N2;
//    const OSMNode* osm_PH;
    std::string name;
    int NodeNum;
    std::pair<std::string, std::string> tagPair;
    
//    int i = 0;
    for (int street = 0; street < numStreets; street++){
        name = getStreetName(street);        
        // from streetid get each street's wayid collection
        for (auto itr = streetDataArray[street].wayIDCollection.begin(); 
            itr != streetDataArray[street].wayIDCollection.end(); itr++){    
            
            osmway = ((OSMHashW.find((*itr).first))->second);
            nodeSet = &(getWayMembers(osmway));
            int tagCount = getTagCount(osmway);
            
            bool oneway = false;
            float speedLimit = 0;
            int detailLevel = -1;
            for(int tag = 0; tag < tagCount; tag++){
                tagPair = getTagPair(osmway, tag);
                
                if (tagPair.first == "oneway"){
                    if (tagPair.second == "yes") oneway = true;
                }else if(tagPair.first == "maxspeed:advisory"){
                    speedLimit = std::stoi(tagPair.second); 
                }else if(tagPair.first == "highway"){
                    detailLevel = findType(tagPair.second);
                }
            }
            
            NodeNum = (*nodeSet).size();
            double y_min, y_max, x_min, x_max;
            
            if (NodeNum > 1){
                osm_N1 = ((OSMHashN.find((*nodeSet).at(0)))-> second);
                LatLon LL = getNodeCoords(osm_N1);
                double y1 = LatttoY (LL.latitude());
                double x1 = LontoX (LL.longitude(), aveLat);
                double y2, x2;
                double angle = 0.00;
                
                int middlenode = round(NodeNum/2.0)-1;
                bool mi;
//                bool test = true;
                StreetSegmentIdx segId = 0;
                for(int node = 1; node < NodeNum; node++){
                    if ((*itr).second[(*nodeSet).at(node-1)]!=0){
                        segId=(*itr).second[(*nodeSet).at(node-1)];
                    }
                    
                    osm_N2 = ((OSMHashN.find((*nodeSet).at(node)))->second);
                    LatLon LL1 = getNodeCoords(osm_N2);
                    y2 = LatttoY (LL1.latitude());
                    x2 = LontoX (LL1.longitude(), aveLat);
                    //store before update coords
                    //store needs to locate the tiles first
                    mi = ( node == middlenode); 
                    
                    if(x1>x2){
                        x_max = x1;
                        x_min = x2;
                    }else{
                        x_max = x2;
                        x_min = x1;
                    }
                    if(y1>y2){
                        y_max = y1;
                        y_min = y2;
                    }else{
                        y_max = y2;
                        y_min = y1;
                    }
                    
                    int yminTileIdx = (y_min-minY)/Tileh;
                    int xminTileIdx = (x_min-minX)/Tilew;
                    int ymaxTileIdx = (y_max-minY)/Tileh;
                    int xmaxTileIdx = (x_max-minX)/Tilew;
                    // here a boundary check is missing, could be added later
                    if(!mi){
                        for (int y_tile = yminTileIdx; y_tile <= ymaxTileIdx; y_tile++){
                            for (int x_tile = xminTileIdx; x_tile<=xmaxTileIdx; 
                                    x_tile++){
                                layerStreetDataMap[detailLevel][y_tile][x_tile].push_back(
                                {street, segId, mi, oneway, speedLimit,name,
                                        0, x1, y1, x2, y2});
                            }
                        }
                    }else{
                        angle = atan2(y2 - y1, x2 - x1) * 180.0 / PI;
                        if(angle > 90.0) angle = angle -180.0;
                        if(angle < -90.0) angle = 180.0 + angle;
                        
                        for (int y_tile = yminTileIdx; y_tile <= ymaxTileIdx; y_tile++){
                            for (int x_tile = xminTileIdx; x_tile<=xmaxTileIdx; 
                                    x_tile++){
                                layerStreetDataMap[detailLevel][y_tile][x_tile].push_back(
                                {street, segId, mi, oneway, speedLimit,name,
                                        angle, x1, y1, x2, y2});
                            }
                        }
                    }
//                    if(test){
                    streetsegEdge[segId].push_back({x1, y1, x2, y2});//}
//                    test = true;
                    y1 = y2;
                    x1 = x2;
                }
            }
        }
        name.erase(std::remove(name.begin(), name.end(), ' '), name.end());
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        streetNameIdMap.insert({name, street});
    }
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "load intersection street time: " << elapsed_seconds.count() << "s\n";
}

int findType(std::string category){
    if(category == "motorway") return 0;
    else if(category == "trunk") return 1;
    else if(category == "primary") return 2;
    else if(category == "secondary") return 3;
    else if(category == "tertiary") return 4;
    else if(category == "unclassified") return 5;
    else if(category == "residential") return 6;
    else {return 7;}
}

void draw_intersections (ezgl::renderer *g){
    // chrono runtime measurement - beginning
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    g->set_color (0, 0, 0, 255);
    std::vector<std::pair<int, int>> highlightList;
    for (int tileY = yTilemin; tileY<=yTilemax; tileY++){
        for (int tileX = xTilemin; tileX<=xTilemax; tileX++){
            for(size_t int_idx = 0; int_idx < intersectionMap[tileY][tileX].size()
                    ; int_idx++){
                double x = intersectionMap[tileY][tileX][int_idx].xPos;
                double y = intersectionMap[tileY][tileX][int_idx].yPos;
                IntersectionIdx id = intersectionMap[tileY][tileX][int_idx].id;
                bool highlight = intersectionInfoArray[id].highlight;
                if(highlight && (id != startNode)) {
                    highlightList.push_back(std::make_pair(x,y));
                }
            }
        }
    }
    // draw highlighted intersection on the very top 
    double x;
    double y;
    for (int inters = 0; inters < highlightList.size(); inters++){
        x = highlightList[inters].first;
        y = highlightList[inters].second;   
        g->draw_surface(my_icon,{x,y},0.0095);

    }
    
    if(startNode!=-1){
        x = intersectionInfoArray[startNode].x;
        y = intersectionInfoArray[startNode].y;
        g->draw_surface(pin,{x,y});
    }
    
    // chrono runtime measurement - ending
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "intersection drawing time: " << elapsed_seconds.count() << "s\n";

}


void draw_streets(ezgl::renderer *g){
    // chrono runtime measurement - beginning
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    g->format_font ("Noto San Georgian", ezgl::font_slant::normal, 
            ezgl::font_weight::normal, 13);
//    float width = 100;  // This is now in m! Understandable
//    float height = width;
    g->set_color (0, 0, 255, 255);
    g->set_line_cap(ezgl::line_cap::round);

    std::vector<NodeSegData> highlightList;
    
    std::unordered_map<std::string, textData> mapmap ;
//    
    for (int tileY = yTilemin; tileY<=yTilemax; tileY++){
        for (int tileX = xTilemin; tileX<=xTilemax; tileX++){
            for(int type = 7; type >= 0; type--){
                if(ratio[type] >= zoomPropotion){
                    draw_level(g, type, tileY, tileX, highlightList, mapmap);
                }
            }
        }
    }
    
    g->set_line_width (6);
    g->set_color(ezgl::color(0,176,255,220));
//    g->set_line_cap(ezgl::line_cap::round);
//    g->set_line_dash(ezgl::line_dash::asymmetric_5_3);
    for(int i = 0; i < edge_col.size(); i++){
        g->draw_line({edge_col[i].x1,edge_col[i].y1},
        {edge_col[i].x2,edge_col[i].y2});
//        std::cout << "draw edge"<<std::endl;
    }
    
    if(lmode){
        g->set_color(backgroundD, 170);
    }else{
        g->set_color(backgroundC, 170);
    }
    for(auto i: mapmap){
        g->set_text_rotation(i.second.angle);
        g->draw_text({i.second.x, i.second.y},i.first,i.second.r, 1000);
    }
    
    g->set_text_rotation(0);
    
    if(zoomPropotion <= 4){
        g->set_color(pathdirec);
        g->format_font ("Noto San Georgian", ezgl::font_slant::normal, 
                ezgl::font_weight::bold, 20);
        double x_draw, y_draw;
        for(int i = 0; i<arrow_col.size(); i++){
            g->set_text_rotation(arrow_col[i].rotation);
            x_draw = intersectionInfoArray[arrow_col[i].node].x;
            y_draw = intersectionInfoArray[arrow_col[i].node].y;
            g->draw_text({x_draw, y_draw},">");
        }
        g->set_text_rotation(0);
    }
    // highlight streets here - by traversing through the highlighted list
    
    // chrono runtime measurement - ending
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "street drawing time: " << elapsed_seconds.count() << "s\n";
}

void draw_level(ezgl::renderer *g, int level, int tileY, int tileX, 
        std::vector<NodeSegData> & highlightList, 
        std::unordered_map<std::string, textData> & mapmap){
    for(int int_idx = 0; int_idx < layerStreetDataMap[level][tileY][tileX].size();
                    int_idx++){
        struct NodeSegData segData = layerStreetDataMap[level][tileY][tileX][int_idx];
        StreetIdx id = segData.id;
        bool highlight = streetDataArray[id].highlight;
        if(highlight) {
                    highlightList.push_back(segData);
        }else{  // if not highlighted just draw in the background
            if (lmode) {
                mapmapInsertName(g, ratio[level], waywidth[level], 
                        waywidthmin[level], lightcolor[level], nameZoomLevel[level],
                        1.5, segData, mapmap);
            }else{
                mapmapInsertName(g, ratio[level], waywidth[level], 
                        waywidthmin[level], darkcolor[level], nameZoomLevel[level],
                        1.5, segData, mapmap);   
            }
        }
    }
}   

void mapmapInsertName(ezgl::renderer *g, double zoomLevel, double widthN, 
        double widthMin, ezgl::color roadColor, double nameZoomL, double onewayzoom,
        NodeSegData& segData, std::unordered_map<std::string, textData>  & mapmap){

    double max_zoom = std::min(zoomPropotion, zoomLevel);
    
    double widthl = (zoomLevel-max_zoom)/zoomLevel * widthN+ widthMin;
    g->set_line_width (widthl);
    
    g->set_color(roadColor);
    g->draw_line({segData.xPos1,segData.yPos1},{segData.xPos2,segData.yPos2});
     if(zoomPropotion <= nameZoomL){
        if(segData.middle){
            StreetSegmentInfo info = getStreetSegmentInfo(segData.segId);
            double x1w = intersectionInfoArray[info.from].x;
            double y1w = intersectionInfoArray[info.from].y;
            double x2w = intersectionInfoArray[info.to].x;
            double y2w = intersectionInfoArray[info.to].y;
            
            double x1 = segData.xPos1;
            double y1 = segData.yPos1;
            double x2 = segData.xPos2;
            double y2 = segData.yPos2;
            
            double radius = sqrt((x1w-x2w)*(x1w-x2w)+(y1w-y2w)*(y1w-y2w));
    
            std::string nameSeg = segData.name;
            if(nameSeg != "<unknown>"){
                if(zoomPropotion < onewayzoom){
                    if(segData.OneWay){
                        if(x1<x2){
                            nameSeg = nameSeg + " =>";
                        }else if(x2<x1){
                            nameSeg = "<=" + nameSeg;
                        }else if(y1>y2){
                            nameSeg = nameSeg + " =>";
                        }else{
                            nameSeg = "<=" + nameSeg;
                        }
                    }
                }
                struct textData dataPush = {(x1+x2)/2,(y1+y2)/2,
                    segData.angle, radius*2, widthl};
                mapmap[nameSeg] = dataPush;
            }
        }
    }
}

std::string oneway_string (std::string nameSeg, bool oneway, double x1,
        double x2, double y1, double y2){
    if (oneway){
        if(x1 < x2){
            nameSeg = nameSeg + " =>";
        }else if(x2<x1){
            nameSeg = "<=" + nameSeg;
        }else if(y1>y2){
            nameSeg = nameSeg + " =>";
        }else{
            nameSeg = "<=" + nameSeg;
        }
    }
    
    return nameSeg;
}

int isPrefix(std::string street_name, std::string test_prefix){
    return test_prefix.compare(street_name.substr(0, test_prefix.length()));
}

int findLowerBound(std::map<std::string, int> m, std::string street_prefix){
    int left = 0;
    int right = m.size()-1;
    int mid;
    auto it = m.begin();
    while(left <= right){
        mid = (left + right) / 2;
        if (right == left){
            if (isPrefix(std::next(it, mid)->first, street_prefix) == 0){
                return left;
            }
            return -1;
        }
        if (isPrefix(std::next(it, mid)->first, street_prefix) == 0){
            right = mid;
        }
        else if (isPrefix(std::next(it, mid)->first, street_prefix) < 0){
            right = mid - 1;
        }
        else if (isPrefix(std::next(it, mid)->first, street_prefix) > 0){
            left = mid + 1;
        }
    }
    return -1;
}

int findUpperBound(std::map<std::string, int> m, std::string street_prefix){
    int left = 0;
    int right = m.size()-1;
    int mid;
    auto it = m.begin();
    while(left <= right){
        mid = (left + right + 1) / 2;
        if (left == right){
            if (isPrefix(std::next(it, mid)->first, street_prefix) == 0){
                return left;
            }
            return -1;
        }
        if (isPrefix(std::next(it, mid)->first, street_prefix) == 0){
            left = mid;
        }
        else if (isPrefix(std::next(it, mid)->first, street_prefix) < 0){
            right = mid - 1;
        }
        else if (isPrefix(std::next(it, mid)->first, street_prefix) > 0){
            left = mid + 1;
        }
    }
    return -1;
}

void clearIntersectionStreets(){
    intersectionMap.clear();
    intersectionInfoArray.clear();
    streetDataArray.clear();
    layerStreetDataMap.clear();
    streetNameIdMap.clear();
    intersectionsOfStreet.clear();
}