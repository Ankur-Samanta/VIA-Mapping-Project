
#ifndef CONTROL_LAYER_H
#define CONTROL_LAYER_H

#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <ctime>
#include <vector>
#include <map>
#include <set>

#include "m1.h"
#include "OSMDatabaseAPI.h"
#include "StreetsDatabaseAPI.h"

#include "ezgl/application.hpp"
#include "ezgl/control.hpp"
#include "ezgl/callback.hpp"
#include "ezgl/graphics.hpp"
#include "POIFunc.h"
#include "intersectionStreetFunc.h"
#include "tileMapConfig.h"
#include "email/email_sender.h"

struct arrow{
    IntersectionIdx node;
    double rotation;
};

void initial_setup(ezgl::application* app, bool);
void setCheckToggleStatus(ezgl::application* app);
void loadIconPng(ezgl::application* app);

extern ezgl::renderer *rendererP;
extern std::vector<ezgl::surface*> POIIcon;
extern ezgl::surface* my_icon;
extern ezgl::surface* pin;
extern double longitude;
extern double latitude;
extern std::string intName;
extern bool lmode;
extern bool mapInputCorrect;
extern IntersectionIdx startNode;

extern std::vector<simpleEdge> edge_col;
extern std::vector<arrow> arrow_col;

static constexpr ezgl::color backgroundD (0x1B, 0x25, 0x35); 
static constexpr ezgl::color pathdirec (13, 83, 145);

extern GtkWidget * dialogHelp;
gboolean CheckEvent (GtkWidget *button, gpointer data);
gboolean press_search(GtkWidget *, gpointer data);
gboolean press_intersection_finder(GtkWidget *, gpointer data);
gboolean change_mode(GtkWidget *, gpointer data);
gboolean email_send(GtkWidget *, gpointer data);
gboolean press_search_bar_1_button(GtkWidget *, gpointer data);
gboolean press_search_bar_2_button(GtkWidget *, gpointer data);
void on_dialog_close(GtkDialog *dialog, gint response_id, gpointer user_data);
void load_map_from_map_name(std::string mapName);
void clearIcon();
std::set<std::string> partialCityName(std::string queryCity);


//std::vector<std::pair<IntersectionIdx, IntersectionIdx>> highlightedNodes;
std::vector<StreetSegmentIdx> drawPath(IntersectionIdx start, IntersectionIdx end);
gboolean press_navigation_button(GtkWidget *, gpointer data);
gboolean press_info_button(GtkWidget *, gpointer data);
//GtkWidget *AddPage (GtkWidget *notebook, const char *szName);
std::vector<std::string> getDirections(std::vector<StreetSegmentIdx> segment, 
        IntersectionIdx startNode);
double AngleCalcIntersection(StreetSegmentIdx edgeidx, StreetSegmentInfo edge, IntersectionIdx StartN);
double calculateAngleBetweenVectors(StreetSegmentInfo prevEdge, 
        StreetSegmentInfo currEdge, StreetSegmentIdx previdx, 
        StreetSegmentIdx curridx);
#endif /* CONTROL_LAYER_H */

