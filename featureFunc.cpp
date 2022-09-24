#include "tileMapConfig.h"
#include "featureFunc.h"
#include "intersectionStreetFunc.h"

std::vector<std::string> featureNames;
std::vector<std::vector<std::vector<featureData>>> featureDataMap;
std::vector<featureData> bigFeatures;
std::unordered_set<featureData,featureData> features;

void loadFeatureMap(){ 
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    std::vector<ezgl::point2d> points;
    int numPoints;
    int drawType;
    FeatureType featureType;
    double x, y, prevX, prevY;
    LatLon LL;
    int numFeat = getNumFeatures();
    featureDataMap.resize(yTileNum);
    featureNames.resize(numFeat);
    for (int y_T = 0; y_T < yTileNum; y_T++){
        featureDataMap[y_T].resize(xTileNum);
    }
    double y_min, y_max, x_min, x_max;
    int yminTileIdx, xminTileIdx, ymaxTileIdx, xmaxTileIdx;
            
    for (int i=0;i<numFeat;i++){
        featureNames[i]==getFeatureName(i);
        featureType=getFeatureType(i);
        
        numPoints=getNumFeaturePoints(i);
        points=std::vector<ezgl::point2d>();
        
        //point
        if (numPoints==1){ 
            drawType = 0;
            LL = getFeaturePoint(0,i);
            y = LatttoY (LL.latitude());
            x = LontoX (LL.longitude(), aveLat);
            points.push_back(ezgl::point2d(x,y));
            
            int yTileIdx = (y-minY)/Tileh;
            int xTileIdx = (x-minX)/Tilew;
            featureDataMap[yTileIdx][xTileIdx].push_back({points, drawType, featureType, i});
        
        //polygon
        }else if (getFeaturePoint(0,i)==getFeaturePoint(numPoints-1,i)){ 
            drawType=2;
            
            y_min=maxY;
            y_max=minY;
            x_min=maxX;
            x_max=minX;
            for(int pointNum=0; pointNum<numPoints; pointNum++){
                LL = getFeaturePoint(pointNum,i);
                y = LatttoY (LL.latitude());
                x = LontoX (LL.longitude(), aveLat);
                points.push_back(ezgl::point2d(x,y));
                
                if (y<y_min)y_min=y;
                if (y>y_max)y_max=y;
                if (x<x_min)x_min=x;
                if (x>x_max)x_max=x;
            }
            
            if ((x_max-x_min)*(y_max-y_min)>=bigFeatureArea){
                bigFeatures.push_back({points, drawType, featureType, i});
            }else{
                yminTileIdx = (y_min-minY)/Tileh;
                xminTileIdx = (x_min-minX)/Tilew;
                ymaxTileIdx = (y_max-minY)/Tileh;
                xmaxTileIdx = (x_max-minX)/Tilew;
                for (int y_tile = yminTileIdx; y_tile <= ymaxTileIdx; y_tile++){
                    for (int x_tile = xminTileIdx; x_tile<=xmaxTileIdx; x_tile++){
                        featureDataMap[y_tile][x_tile].push_back(
                            {points, drawType, featureType, i});
                    }
                }
            }
            
        //node segment
        }else{ 
            drawType=1;
            LL = getFeaturePoint(0,i);
            prevY = LatttoY (LL.latitude());
            prevX = LontoX (LL.longitude(), aveLat);
            for(int pointNum=1; pointNum<numPoints; pointNum++){
                LL = getFeaturePoint(pointNum,i);
                y = LatttoY (LL.latitude());
                x = LontoX (LL.longitude(), aveLat);
                points=std::vector<ezgl::point2d>();
                points.push_back(ezgl::point2d(prevX,prevY));
                points.push_back(ezgl::point2d(x,y));
                
                if(x>prevX){
                    x_max = x;
                    x_min = prevX;
                }else{
                    x_max = prevX;
                    x_min = x;
                }
                if(y>prevY){
                    y_max = y;
                    y_min = prevY;
                }else{
                    y_max = prevY;
                    y_min = y;
                }
                yminTileIdx = (y_min-minY)/Tileh;
                xminTileIdx = (x_min-minX)/Tilew;
                ymaxTileIdx = (y_max-minY)/Tileh;
                xmaxTileIdx = (x_max-minX)/Tilew;

                for (int y_tile = yminTileIdx; y_tile <= ymaxTileIdx; y_tile++){
                    for (int x_tile = xminTileIdx; x_tile<=xmaxTileIdx; x_tile++){
                        featureDataMap[y_tile][x_tile].push_back(
                            {points, drawType, featureType, i});
                    }
                }
                
                prevX=x;
                prevY=y;
            }
        }
        //{points, drawType, getFeatureType(i), i}
    }
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "load features time: " << elapsed_seconds.count() << "s\n";
}

void draw_features (ezgl::renderer *g){
////    chrono runtime measurement - beginning
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    std::vector<featureData> Lake;
    std::vector<featureData> island;
    std::vector<featureData> river;
    
    features.clear();    
    // tile map features
    for (int tileY = yTilemin; tileY<=yTilemax; tileY++){
        for (int tileX = xTilemin; tileX<=xTilemax; tileX++){
            for(size_t int_idx = 0; int_idx < featureDataMap[tileY][tileX].size(); int_idx++){
                FeatureType featType = featureDataMap[tileY][tileX][int_idx].featureType;
                bool ins = false;
                if(featType == 0){
                    
                    ins = true;
                
                }else if(featType == 1){
                    
                    if(zoomPropotion <= parkRatio) ins = true;
                
                }else if(featType == 2){
                    
                    if(zoomPropotion <= beachRatio) ins = true;
                
                }else if(featType == 3){

                    if(zoomPropotion <= lakeRatio){
                        ins = false;
                        Lake.push_back(featureDataMap[tileY][tileX][int_idx]);}
                
                }else if(featType == 4){
                    
                    if(zoomPropotion <= riverRatio) {
                        ins = false;
                        river.push_back(featureDataMap[tileY][tileX][int_idx]);
                    }
                
                }else if(featType == 5){
                    
                    ins = false;
                    island.push_back(featureDataMap[tileY][tileX][int_idx]);
                }else if(featType == 6){
                    
                    if(zoomPropotion <= buildingRatio) ins = true;
                
                }else if(featType == 7){
                    
                    if(zoomPropotion <= greenspaceRatio) ins = true;
                
                }else if(featType == 8){
                    
                    if(zoomPropotion <= golfRatio) ins = true;
                
                }else if(featType == 9){
                    
                    if(zoomPropotion <= streamRatio) ins = true;
                
                }else if(featType == 10){
                    
                    if(zoomPropotion <= glacierRatio) ins = true;
                
                }
                
                if(ins) features.insert(featureDataMap[tileY][tileX][int_idx]);
            }
        }
    }
    
    // big features
    for(std::vector<featureData>::iterator itr=bigFeatures.begin();
            itr!=bigFeatures.end();itr++){
        if((*itr).featureType == 3){
            Lake.push_back(*itr);
        }else if((*itr).featureType == 5){
            island.push_back(*itr);
        }else if((*itr).featureType == 4){
            river.push_back(*itr);
        }else{
            features.insert(*itr);
        }
    }
    
    // draw all features
//    g->set_color (0, 255, 0, 255);
    std::vector<ezgl::point2d> points;
    
    // lake first
    drawFeatureByCategory(g, lakeCL, lakeCD, Lake);
    
    // island
    drawFeatureByCategory(g, islandCL, islandCD, island);

    // river
    drawFeatureByCategory(g, riverCL, riverCD, river);

    
    
    if (lmode) { //default light mode
        for(std::unordered_set<featureData,featureData>::iterator itr=features.begin();
                itr!=features.end();itr++){
            FeatureType featType = itr->featureType;

            if(featType == 0){
                g->set_color (backgroundC); //keep as backgroundC

            }else if(featType == 1){

                g->set_color (parkCL);

            }else if(featType == 2){

                g->set_color (beachCL);

            }else if(featType == 3){

                g->set_color (lakeCL);

            }else if(featType == 4){
                
                g->set_color (riverCL);

            }else if(featType == 5){

                g->set_color (islandCL);

            }else if(featType == 6){

                g->set_color (buildingCL);

            }else if(featType == 7){

                g->set_color (greenspaceCL);

            }else if(featType == 8){

                g->set_color (golfCL);

            }else if(featType == 9){

                g->set_color (streamCL);

            }else if(featType == 10){

                g->set_color (glacierCL);

            }
            points=itr->points;
            //point
            if (itr->drawType==0){
                g->fill_arc(points[0],featureR, 0, 360);
            //node segment
            }else if (itr->drawType==1){ 
                g->draw_line(points[0], points[1]);
            //polygon
            }else{ 
                g->fill_poly(points);
            }
        }
    }
    else {
        for(std::unordered_set<featureData,featureData>::iterator itr=features.begin();
                itr!=features.end();itr++){
            FeatureType featType = itr->featureType;

            if(featType == 0){
                g->set_color (backgroundC);

            }else if(featType == 1){

                g->set_color (parkCD);

            }else if(featType == 2){

                g->set_color (beachCD);

            }else if(featType == 3){

                g->set_color (lakeCD);

            }else if(featType == 4){

                g->set_color (riverCD);

            }else if(featType == 5){

                g->set_color (islandCD);

            }else if(featType == 6){

                g->set_color (buildingCD);

            }else if(featType == 7){

                g->set_color (greenspaceCD);

            }else if(featType == 8){

                g->set_color (golfCD);

            }else if(featType == 9){

                g->set_color (streamCD);

            }else if(featType == 10){

                g->set_color (glacierCD);

            }
            points=itr->points;
            //point
            if (itr->drawType==0){
                g->fill_arc(points[0],featureR, 0, 360);
            //node segment
            }else if (itr->drawType==1){ 
                g->draw_line(points[0], points[1]);
            //polygon
            }else{ 
                g->fill_poly(points);
            }
        }
    }
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "feature drawing time: " << elapsed_seconds.count() << "s\n";
}

void drawFeatureByCategory(ezgl::renderer *g, ezgl::color lightCol, ezgl::color darkCol, std::vector<featureData> & featArr){
    if(lmode){
        g->set_color(lightCol);
    }else{
        g->set_color(darkCol);
    }
    std::vector<ezgl::point2d> points;
    
    for(int i = 0; i< featArr.size();i++){
        points=featArr[i].points;
        //point
        if (featArr[i].drawType==0){
            g->fill_arc(points[0],featureR, 0, 360);
        //node segment
        }else if (featArr[i].drawType==1){ 
            g->draw_line(points[0], points[1]);
        //polygon
        }else{ 
            g->fill_poly(points);
        }
    }
}

void clearFeatures(){
    featureNames.clear();
    featureDataMap.clear();
    bigFeatures.clear();
}