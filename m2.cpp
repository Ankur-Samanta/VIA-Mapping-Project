
#include <iostream>
#include <algorithm>
#include <string>
#include <chrono>
#include <ctime>
#include <cmath>
#include <map>
#include <set>

#include "m1.h"
#include "m2.h"
#include "StreetsDatabaseAPI.h"
#include "OSMDatabaseAPI.h"
#include "m2Helper/controlLayer.h"
#include "m2Helper/tileMapConfig.h"
#include "m2Helper/intersectionStreetFunc.h"
#include "m2Helper/featureFunc.h"
#include "m2Helper/POIFunc.h"
#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"
#include "ezgl/callback.hpp"
#include "graphics.hpp"


double viewWidth;
double viewHeight;
double zoomPropotion;
IntersectionIdx startNode;
constexpr double mouseThreshold = 30.0;
std::vector<IntersectionIdx> findIntersections;
std::vector<simpleEdge> edge_col;


intersectionData highlight_int = {-1, -1, -1};
void draw_main_canvas (ezgl::renderer *g);
double xydis_twopoints(double x1, double y1, double x2, double y2);
void act_on_mouse_click(ezgl::application* app,   
                        GdkEventButton* event,
                        double x, double y);

void drawMap () {
    startNode = -1;
    
    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";
    //settings.notebook_identifier = "MainNotebook";
    ezgl::application application(settings);
   
    ezgl::rectangle initial_world({minX, minY}, {maxX, maxY});
    
    lmode = true;
    application.add_canvas("MainCanvas", draw_main_canvas, initial_world, backgroundC);
//    loadIconPng(&application);
    application.run(initial_setup, act_on_mouse_click, nullptr, nullptr);
    //application.run(initial_setup, act_on_mouse_clock, act_on_mouse_move, act_on_key_press);

}

void draw_main_canvas (ezgl::renderer *g) {
    rendererP = g;
    // check the visible box and only draw the required tiles according to the 
    // visible box config
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    
    ezgl::rectangle visibleBox = g->get_visible_world();
//    g->set_color(ezgl::RED);
//    g->draw_rectangle({minX, minY}, {maxX, maxY});
    
     //ezgl::surface* my_icon_surface=g->load_png("my_icon.png");
     //ezgl::surface* my_icon_surface = load_png("libstreetmap/resources/my_icon.png");
    g->set_color(backgroundD);
    if(!lmode) g->fill_rectangle({minX, minY}, {maxX, maxY}); 
    
    
    double minimumX = visibleBox.left();
    double maximumX = visibleBox.right();
    double minimumY = visibleBox.bottom();
    double maximumY = visibleBox.top();
    
    ezgl::rectangle visiblescreen = g->get_visible_screen();
    
    double minimumXs = visiblescreen.left();
    double maximumXs = visiblescreen.right();
    double minimumYs = visiblescreen.bottom();
    double maximumYs = visiblescreen.top();
    
    std::cout<<"the screen config is"<<(maximumXs-minimumXs)*(maximumYs-minimumYs)<<std::endl;
    std::cout<<"the world area is"<<(maximumX-minimumX)*(maximumY-minimumY)<<std::endl;
    
    zoomPropotion = ((maximumX-minimumX)*(maximumY-minimumY))/
            ((maximumXs-minimumXs)*(maximumYs-minimumYs));
    
    viewWidth = maximumX - minimumX;
    viewHeight = maximumY - minimumY;
    
    zoomRatio = (viewWidth * viewHeight-MinZoomArea)/(MaxZoomArea-MinZoomArea);
    zoomRatio = std::min((double)std::max(zoomRatio,0.0),1.0);
    std::cout<<"zoom% is "<< zoomPropotion <<std::endl;
    
    int xminTileIdx = (minimumX -minX)/Tilew;
    int xmaxTileIdx = (maximumX -minX)/Tilew;
    int yminTileIdx = (minimumY -minY)/Tileh;
    int ymaxTileIdx = (maximumY -minY)/Tileh;
    
    // make sure the tiles does not go out of bound for vector indices
    xTilemin = std::max(xminTileIdx, 0);
    xTilemax = std::min(xmaxTileIdx, xTileNum-1);
    yTilemin = std::max(yminTileIdx, 0);
    yTilemax = std::min(ymaxTileIdx, yTileNum-1);
    
    // background rendering
//    g->set_color(232, 234, 237, 255);
//    g->set_color(ezgl::RED);
//    g->draw_rectangle({minX, minY}, {maxX, maxY});
    
    // intersection drawing
    draw_features(g);
    draw_streets(g);
    if(POIIcon.size()!=0){
//        std::cout<<"really"<<std::endl;
        if(zoomPropotion <= 1.4)
            draw_POI(g);
    }
    draw_intersections(g);
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "total drawing time: " << elapsed_seconds.count() << "s\n\n";
    
}

void act_on_mouse_click(ezgl::application* app,   
                        GdkEventButton* event,
                        double x, double y) {    

    //Ankur:added below implementation specifically for buttons
//    std::cout << "Mouse clicked (" << x << "," << y << ")\n";
    
    
    //Lindy:implementation for highlights resumes below

    // if highlight:
        
    double minimumX = x - mouseThreshold/2;
    double maximumX = x + mouseThreshold/2;
    double minimumY = y - mouseThreshold/2;
    double maximumY = y + mouseThreshold/2;
    
    int xminTileIdx = (minimumX -minX)/Tilew;
    int xmaxTileIdx = (maximumX -minX)/Tilew;
    int yminTileIdx = (minimumY -minY)/Tileh;
    int ymaxTileIdx = (maximumY -minY)/Tileh;
    
    int xmin = std::max(xminTileIdx, 0);
    int xmax = std::min(xmaxTileIdx, xTileNum-1);
    int ymin = std::max(yminTileIdx, 0);
    int ymax = std::min(ymaxTileIdx, yTileNum-1);
   
    // make sure the tiles does not go out of bound for vector indices
    std::cout<<"========clicking info======="<<std::endl;
    std::cout<<" min x tile: "<< xmin<<std::endl;
    std::cout<<" max x tile: "<< xmax<<std::endl;
    std::cout<<" min y tile: "<< ymin<<std::endl;
    std::cout<<" max y tile: "<< ymax<<std::endl;
    std::cout<<"============================"<<std::endl;
    
    std::cout<<"changing highlight"<<std::endl;
        // if combobox select intersections
    
    // chrono runtime measurement - beginning
//    std::chrono::time_point<std::chrono::system_clock> start, end;
//    start = std::chrono::system_clock::now();
    
    if(intersectionMap[ymin][xmin].size()!=0){
        double x_coord = intersectionMap[ymin][xmin][0].xPos;
        double y_coord = intersectionMap[ymin][xmin][0].yPos;
        double min_dis = xydis_twopoints(x, y, x_coord, y_coord);
        double dis = min_dis;

//        double POIx_coord = POIMap[ymin][xmin][0].xPos;
//        double POIy_coord = POIMap[ymin][xmin][0].yPos;
//        double POImin_dis = xydis_twopoints(x, y, POIx_coord, POIy_coord);
//        double POIdis = POImin_dis;
        
        int highlight_idx = intersectionMap[ymin][xmin][0].id;
//        int POIhighlight_idx = POIMap[ymin][xmin][0].id;

        for (int tileY = ymin; tileY <= ymax; tileY++){
            for (int tileX = xmin; tileX <= xmax; tileX++){
                for(size_t int_idx = 0; int_idx < intersectionMap[tileY][tileX].size()
                        ; int_idx++){
                    x_coord = intersectionMap[tileY][tileX][int_idx].xPos;
                    y_coord = intersectionMap[tileY][tileX][int_idx].yPos;
                    dis = xydis_twopoints(x, y, x_coord, y_coord);
                    if(dis < min_dis){
                        min_dis = dis; 
                        highlight_idx = intersectionMap[tileY][tileX][int_idx].id;
                    }
                }             
//                for(size_t int_idx = 0; int_idx < POIMap[tileY][tileX].size()
//                        ; int_idx++){
//                    POIx_coord = POIMap[tileY][tileX][int_idx].xPos;
//                    POIy_coord = POIMap[tileY][tileX][int_idx].yPos;
//                    POIdis = xydis_twopoints(x, y, POIx_coord, POIy_coord);
//                    if(POIdis < POImin_dis){
//                        POImin_dis = POIdis; 
//                        POIhighlight_idx = POIMap[tileY][tileX][int_idx].id;
//                    }
//                }
            }
        }
        
//        std::cout<<"the closest POI is: "<<POIInfoArray[POIhighlight_idx].name<<std::endl;
//        POIInfoArray[POIhighlight_idx].near_highlight = !POIInfoArray[POIhighlight_idx].near_highlight;
        
        
        if (min_dis <= mouseThreshold){
            std::cout<<"highlight changed"<<std::endl<<std::endl;
            
            auto findinter = std::find(findIntersections.begin(), findIntersections.end(), highlight_idx); 
//            auto findinter = findIntersections.find(highlight_idx);
            
            if (findinter == findIntersections.end()){
                // in this case the intersection is not highlighted
//                double xposition= intersectionInfoArray[highlight_idx].x;
//                double yposition= intersectionInfoArray[highlight_idx].y;
//                intersectionData newdata = {highlight_idx,xposition,yposition};
                findIntersections.push_back(highlight_idx);
                intersectionInfoArray[highlight_idx].highlight = true;
                
                auto itr = intersectionInfoArray[highlight_idx].street_col.begin();
                std::string object_str = "Intersection: " + *itr; itr++;
                while(itr != intersectionInfoArray[highlight_idx].street_col.end()){
                    object_str += (" & " + *itr);
                    itr++;
                }

                gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY
                        (app->get_object("StatusSearch"))), object_str.c_str(), -1);
                    
            }else{
                // in this case the intersection is already highlighted
                findIntersections.erase(findinter); 
                intersectionInfoArray[highlight_idx].highlight = false;
                gtk_entry_buffer_set_text(gtk_entry_get_buffer
                            (GTK_ENTRY(app->get_object("StatusSearch"))),
                            "", -1);
            }
        }
    }
    // chrono runtime measurement - ending
    
//    end = std::chrono::system_clock::now();
//    std::chrono::duration<double> elapsed_seconds = end - start;
//  
//    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
    
    app->refresh_drawing();
} 

double xydis_twopoints(double x1, double y1, double x2, double y2){

    return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

/*
act_on_mouse_move(ezgl::application* app, GdkEventButton* event, double x, double y) {
    //GDK_MOTION_NOTIFY: the pointer (usually a mouse) has moved.
}

act_on_key_press(ezgl::application* app, GdkEventButton* event, double x, double y) {
    event->key
}
*/