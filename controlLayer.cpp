
#include "controlLayer.h"

#include "intersectionStreetFunc.h"
#include "tileMapConfig.h"
#include "m3.h"
#include <queue>
#include <cmath>

std::set<std::string> mapFileNames {"toronto", "tokyo", "tehran", "sydney", "singapore", "saint helena", "sio de janeiro", "new york", "new delhi", "moscow", "london", "interlaken", "iceland", "hong kong", "hamilton", "golden horseshoe", "cape town", "cairo", "beijing"};
std::vector<ezgl::surface*> POIIcon;
std::vector<arrow> arrow_col;

ezgl::surface* my_icon;
ezgl::surface* pin;

bool lmode;
double longitude;
double latitude;
std::string intName;
bool mapInputCorrect;
std::string tempFileName;
double turn_penalty = 15; //used for A* path finding
GtkWidget * dialogHelp;


//utility function to set up initial gtk declarations 
void initial_setup(ezgl::application* app, bool){
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    setCheckToggleStatus(app);
    loadIconPng(app);
    
    longitude = -1.0;
    latitude = -1.0;
    intName = "";
    //Page 1
    
    //dialog////////////////////////////////////////////////
    
    
    GObject *window;
    GtkWidget *content_area;
    
//    GtkWidget *dialog;;
    
    window = app->get_object(app->get_main_window_id().c_str());
    
    dialogHelp = gtk_dialog_new_with_buttons("VIA Help Center", (GtkWindow*) window, 
            GTK_DIALOG_DESTROY_WITH_PARENT, ("OK"), 
            GTK_RESPONSE_ACCEPT, ("CANCEL"), GTK_RESPONSE_REJECT, NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialogHelp));
    gtk_widget_set_size_request(dialogHelp, 500, 500);
    gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(app->get_object("notebookdisplay")));
    gtk_widget_show(dialogHelp);
//    gtk_widget_hide_on_delete(dialogHelp);
//    g_signal_stop_emission_by_name( dialogHelp, "GTK_RESPONSE_DELETE_EVENT");
    g_signal_connect(GTK_DIALOG(dialogHelp), "delete-event", G_CALLBACK(on_dialog_close), NULL);
    g_signal_connect(GTK_DIALOG(dialogHelp), "response", G_CALLBACK(on_dialog_close), NULL);
    
    //////////////////////////////
    
    
    
    
    
    GObject *search_button = app->get_object("Search");
    g_signal_connect(search_button, "clicked", G_CALLBACK(press_search), app);
    
    GObject *intersection_finder_button = app->get_object("clearPathButton");
    g_signal_connect(intersection_finder_button, "clicked", G_CALLBACK(press_intersection_finder), app);
    
    GObject *search_bar_1_button = app->get_object("SearchBar1Button");
    g_signal_connect(search_bar_1_button, "clicked", G_CALLBACK(press_search_bar_1_button), app);
    searchBarStreet1=-1;
    
    GObject *search_bar_2_button = app->get_object("SearchBar2Button");
    g_signal_connect(search_bar_2_button, "clicked", G_CALLBACK(press_search_bar_2_button), app);
    searchBarStreet2=-1;
    
    GObject *navigation_button = app->get_object("Navigation");
    g_signal_connect(navigation_button, "clicked", G_CALLBACK(press_navigation_button), app);
    
    //Page 3
    
    GObject *mode_button = app->get_object("ModeToggle");
    g_signal_connect(mode_button, "toggled", G_CALLBACK(change_mode), app);
    
    GObject *help_button = app->get_object("HELP");
    g_signal_connect(help_button, "clicked", G_CALLBACK(email_send), app);
    
    GObject *info_button = app->get_object("INFO");
    g_signal_connect(info_button, "clicked", G_CALLBACK(press_info_button), app);
    
    
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "initial setup time: " << elapsed_seconds.count() << "s\n";
}

//utility function to load icons
void loadIconPng(ezgl::application* app){
    std::cout<<"loading icons"<<std::endl;
    ezgl::renderer* g = app->get_renderer();
    my_icon = g->load_png("libstreetmap/resources/my_icon.png");
    pin = g->load_png("libstreetmap/resources/pin.png");

    //loading icons for POI 
    ezgl::surface* iconPH = g->load_png("libstreetmap/resources/bar.png");// 0
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/cafe.png"); //1
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/biergarten.png");//2
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/fastfood.png");//3
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/foodcourt.png");//4
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/icecream.png");//5
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/pub.png");//6
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/restaurant.png");//7
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/courthouse.png");//8
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/firestation.png");//9
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/police.png");//10
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/postbox.png");//11
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/postoffice.png");//12
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/prison.png");//13
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/townhall.png");//14
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/artcenter.png");//15
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/casino.png");//16
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/cinema.png");//17
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/communitycenter.png");//18
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/fountain.png"); //19
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/nightclub.png"); //20
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/publicbookcase.png"); //21
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/theatre.png"); //22
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/education.png"); //23
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/atm.png"); //24
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/bank1.png"); //25
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/moneychanger.png"); //26
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/clinic.png"); //27
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/dentist.png"); //28
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/doctors.png"); //29
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/hospital.png"); //30
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/pharmacy.png"); //31
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/socialfacility.png"); //32
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/veterinary.png"); //33
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/bicyleparking.png"); //34
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/bicyclerepair.png"); //35
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/bicylerecycle.png"); //36
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/boatrental.png"); //37
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/busstation.png"); //38
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/carrental.png"); //39
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/carwash.png"); //40
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/vehicleinspection.png"); //41
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/charingstation.png"); //42
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/ferryterminal.png"); //43
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/fuel.png"); //44
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/motorcycleparking.png"); //45
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/parking.png"); //46
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/parkingspace.png"); //47
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/taxi.png"); //48
    POIIcon.push_back(iconPH);
    iconPH = g->load_png("libstreetmap/resources/unassignedicon.png"); //49 for all unassigned icon
    POIIcon.push_back(iconPH);
}

//utility function to check the toggles for each defined POI type and initialize their declarations and callbacks
void setCheckToggleStatus(ezgl::application* app){
    gpointer Entertain = GINT_TO_POINTER (1); //POIIcon[1]
    gpointer Food = GINT_TO_POINTER (0); //POIIcon[0]
    gpointer PublicService = GINT_TO_POINTER (3); //POIIcon[2]
    gpointer Transit = GINT_TO_POINTER (2); //POIIcon[3]
    gpointer Education = GINT_TO_POINTER (4); //POIIcon[4]
    gpointer Traffic = GINT_TO_POINTER (5); //POIIcon[5]
    
    
    GObject *entCheck = app -> get_object("EntertainmentToggle");
    g_object_set_data(G_OBJECT(entCheck), "cate", Entertain);
    g_signal_connect(entCheck, "toggled", G_CALLBACK(CheckEvent), app);
    
    GObject *foodCheck = app -> get_object("FoodToggle");
    g_object_set_data(G_OBJECT(foodCheck), "cate", Food);
    g_signal_connect(foodCheck, "toggled", G_CALLBACK(CheckEvent), app);
    
    GObject *publicServiceCheck = app -> get_object("PublicServicesToggle");
    g_object_set_data(G_OBJECT(publicServiceCheck), "cate", PublicService);
    g_signal_connect(publicServiceCheck, "toggled", G_CALLBACK(CheckEvent), app);
    
    GObject *transitCheck = app -> get_object("TransitToggle");
    g_object_set_data(G_OBJECT(transitCheck), "cate", Transit);
    g_signal_connect(transitCheck, "toggled", G_CALLBACK(CheckEvent), app);
    
    GObject *eduCheck = app -> get_object("EducationToggle");
    g_object_set_data(G_OBJECT(eduCheck), "cate", Education);
    g_signal_connect(eduCheck, "toggled", G_CALLBACK(CheckEvent), app);
    
    GObject *otherCheck = app -> get_object("OtherToggle");
    g_object_set_data(G_OBJECT(otherCheck), "cate", Traffic);
    g_signal_connect(otherCheck, "toggled", G_CALLBACK(CheckEvent), app);


}

//utility function to check event; this is the callback function used for all POI toggles
gboolean CheckEvent (GtkWidget *button, gpointer data) {

    auto application = static_cast<ezgl::application *>(data);
    
    gint caseT = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "cate"));
    
    
    for (int POIn = 0; POIn < POICategoryArr[caseT].size(); POIn++){
            POIIdx POIIndex = POICategoryArr[caseT][POIn]; 
            POIInfoArray[POIIndex].highlight = !POIInfoArray[POIIndex].highlight;
        }
    std::cout<<"do work"<< POICategoryArr[caseT].size() << std::endl;
    application ->refresh_drawing();
    return TRUE;

}

double baseTaxiPrice = 4.25;
double perKmTaxiPrice = 1.75;
std::string priceUnits = "CAD";

//utility function to determine the map filepath from the mapname; this is used to load in a new map; also sets taxi price info
void load_map_from_map_name(std::string mapName) {
    mapInputCorrect = TRUE;
    
    if (mapName == "toronto") {
        tempFileName = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/toronto_canada.streets.bin" << std::endl;
        baseTaxiPrice = 4.25;
        perKmTaxiPrice = 1.75;
        priceUnits = "CAD";
    }
    else if (mapName == "tokyo") {
        tempFileName = "/cad2/ece297s/public/maps/tokyo_japan.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/tokyo_japan.streets.bin" << std::endl;
        baseTaxiPrice = 730;
        perKmTaxiPrice = 320;
        priceUnits = "Yen";
    }
    else if (mapName == "tehran") {
        tempFileName = "/cad2/ece297s/public/maps/tehran_iran.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/tehran_iran.streets.bin" << std::endl;
        baseTaxiPrice = 1.8;
        perKmTaxiPrice = 1.5;
        priceUnits = "Riyal";
    }
    else if (mapName == "sydney") {
        tempFileName = "/cad2/ece297s/public/maps/sydney_australia.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/sydney_australia.streets.bin" << std::endl;
        baseTaxiPrice = 3.60;
        perKmTaxiPrice = 2.19;
        priceUnits = "AUD";
    }
    else if (mapName == "singapore") {
        tempFileName = "/cad2/ece297s/public/maps/singapore.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/singapore.streets.bin" << std::endl;
        baseTaxiPrice = 4.05;
        perKmTaxiPrice = 0.70;
        priceUnits = "SID";
    }
    else if (mapName == "saint helena") {
        tempFileName = "/cad2/ece297s/public/maps/saint-helena.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/saint-helena.streets.bin" << std::endl;
        baseTaxiPrice = 4.10;
        perKmTaxiPrice = 0.63;
        priceUnits = "D";
    }
    else if (mapName == "rio de janeiro") {
        tempFileName = "/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/rio-de-janeiro_brazil.streets.bin" << std::endl;
        baseTaxiPrice = 6.00;
        perKmTaxiPrice = 2.65;
        priceUnits = "Real";
    }
    else if (mapName == "new york") {
        tempFileName = "/cad2/ece297s/public/maps/new-york_usa.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/new-york_usa.streets.bin" << std::endl;
        baseTaxiPrice = 2.50;
        perKmTaxiPrice = 1.56;
        priceUnits = "USD";
    }
    else if (mapName == "new delhi") {
        tempFileName = "/cad2/ece297s/public/maps/new-delhi_india.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/new-delhi_india.streets.bin" << std::endl;
        baseTaxiPrice = 25;
        perKmTaxiPrice = 16;
        priceUnits = "Rupees";
    }
    else if (mapName == "moscow") {
        tempFileName = "/cad2/ece297s/public/maps/moscow_russia.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/moscow_russia.streets.bin" << std::endl;
        baseTaxiPrice = 190;
        perKmTaxiPrice = 15;
        priceUnits = "Rubles";
    }
    else if (mapName == "london") {
        tempFileName = "/cad2/ece297s/public/maps/london_england.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/london_england.streets.bin" << std::endl;
        baseTaxiPrice = 4.20;
        perKmTaxiPrice = 1;
        priceUnits = "Euros";
    }
    else if (mapName == "interlaken") {
        tempFileName = "/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/interlaken_switzerland.streets.bin" << std::endl;
        baseTaxiPrice = 15;
        perKmTaxiPrice = 4;
        priceUnits = "CHF";
    }
    else if (mapName == "iceland") {
        tempFileName = "/cad2/ece297s/public/maps/iceland.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/iceland.streets.bin" << std::endl;
        baseTaxiPrice = 690;
        perKmTaxiPrice = 260;
        priceUnits = "KR";
    }
    else if (mapName == "hong kong") {
        tempFileName = "/cad2/ece297s/public/maps/hong-kong_china.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/hong-kong_china.streets.bin" << std::endl;
        baseTaxiPrice = 24;
        perKmTaxiPrice = 7;
        priceUnits = "HKD";
    }
    else if (mapName == "hamilton") {
        tempFileName = "/cad2/ece297s/public/maps/hamilton_canada.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/hamilton_canada.streets.bin" << std::endl;
        baseTaxiPrice = 4.40;
        perKmTaxiPrice = 1.8;
        priceUnits = "CAD";
    }
    else if (mapName == "golden horseshoe") {
        tempFileName = "/cad2/ece297s/public/maps/golden-horseshoe_canada.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/golden-horseshoe_canada.streets.bin" << std::endl;
        baseTaxiPrice = 4.25;
        perKmTaxiPrice = 1.75;
        priceUnits = "CAD";
    }
    else if (mapName == "cape town") {
        tempFileName  = "/cad2/ece297s/public/maps/cape-town_south-africa.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/cape-town_south-africa.streets.bin" << std::endl;
        baseTaxiPrice = 20;
        perKmTaxiPrice = 10;
        priceUnits = "Rand";
    }
    else if (mapName == "cairo") {
        tempFileName = "/cad2/ece297s/public/maps/cairo_egypt.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/cairo_egypt.streets.bin" << std::endl;
        baseTaxiPrice = 5;
        perKmTaxiPrice = 2.50;
        priceUnits = "EGP";
    }
    else if (mapName == "beijing") {
        tempFileName = "/cad2/ece297s/public/maps/beijing_china.streets.bin";// tokyo_japan
        std::cout << "/cad2/ece297s/public/maps/beijing_china.streets.bin" << std::endl;
        baseTaxiPrice = 0;
        perKmTaxiPrice = 4.3;
        priceUnits = "RMB";
    }
    else {
        mapInputCorrect = FALSE; //if city entered is not in database
    }
}

//utility function to perform autocomplete on partial city names
std::set<std::string> partialCityName(std::string queryCity) {
    std::set<std::string> tempNames;
    for (auto elem : mapFileNames) {
        if (queryCity.size() <= elem.size()) {
            if (std::mismatch(queryCity.begin(), queryCity.end(), elem.begin()).first == queryCity.end()) {
                tempNames.insert(elem);
            }
        }
    }
    return tempNames;
}

std::string queryCity;

//utility function dealing with buttons for the popup dialog boxes in gtk
void on_dialog_response(GtkDialog *dialog, gint response_id, gpointer user_data)
{
    // For demonstration purposes, this will show the enum name and int value of the button that was pressed
    std::cout << "response is ";
    switch(response_id) {
        case GTK_RESPONSE_ACCEPT:
            std::cout << "GTK_RESPONSE_ACCEPT ";
            break;
        case GTK_RESPONSE_DELETE_EVENT:
            std::cout << "GTK_RESPONSE_DELETE_EVENT (i.e. X button) ";
            break;
        case GTK_RESPONSE_REJECT:
            std::cout << "GTK_RESPONSE_REJECT ";
            break;
        default:
            std::cout << "UNKNOWN ";
            break;
    }
    std::cout << "(" << response_id << ")\n";
    // This will cause the dialog to be destroyed and close.
    // without this line the dialog remains open unless the
    // response_id is GTK_RESPONSE_DELETE_EVENT which
    // automatically closes the dialog without the following line.
    gtk_widget_destroy(GTK_WIDGET (dialog));
}

void on_dialog_hide(GtkDialog *dialog, gint response_id, gpointer user_data)
{
    // For demonstration purposes, this will show the enum name and int value of the button that was pressed
    std::cout << "response is ";
    switch(response_id) {
        case GTK_RESPONSE_ACCEPT:
            std::cout << "GTK_RESPONSE_ACCEPT ";
            break;
        case GTK_RESPONSE_DELETE_EVENT:
            std::cout << "GTK_RESPONSE_DELETE_EVENT (i.e. X button) ";
            break;
        case GTK_RESPONSE_REJECT:
            std::cout << "GTK_RESPONSE_REJECT ";
            break;
        default:
            std::cout << "UNKNOWN ";
            break;
    }
    std::cout << "(" << response_id << ")\n";
    // This will cause the dialog to be destroyed and close.
    // without this line the dialog remains open unless the
    // response_id is GTK_RESPONSE_DELETE_EVENT which
    // automatically closes the dialog without the following line.
    gtk_widget_hide(GTK_WIDGET (dialog));
}



//utility function dealing with buttons for the popup dialog boxes in gtk
void on_dialog_close(GtkDialog *dialog, gint response_id, gpointer user_data)
{
    // For demonstration purposes, this will show the enum name and int value of the button that was pressed
    std::cout << "response is ";
    switch(response_id) {
        case GTK_RESPONSE_ACCEPT:
            std::cout << "GTK_RESPONSE_ACCEPT ";
            break;
        case GTK_RESPONSE_DELETE_EVENT:
            std::cout << "GTK_RESPONSE_DELETE_EVENT";
            break;
        case GTK_RESPONSE_REJECT:
            std::cout << "GTK_RESPONSE_REJECT ";
            break;
        default:
            std::cout << "UNKNOWN ";
            break;
    }
    std::cout << "(" << response_id << ")\n";
    // This will cause the dialog to be destroyed and close.
    // without this line the dialog remains open unless the
    // response_id is GTK_RESPONSE_DELETE_EVENT which
    // automatically closes the dialog without the following line.
    gtk_widget_hide(dialogHelp);
}


//utility callback function to define what happens when a search bar is pressed. This largely handles autocomplete for both search bars
gboolean press_search(GtkWidget *, gpointer data) {
    mapInputCorrect = TRUE; //reset parameter
    auto app = static_cast<ezgl::application *>(data);
    queryCity = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("CityNameSearch"))));
    std::transform(queryCity.begin(), queryCity.end(), queryCity.begin(), [](unsigned char c){return std::tolower(c);});
    std::cout << queryCity << std::endl;
    
    //find the map names that can have the entered text as a prefix, and overwrite search bar if unique match is found
    std::set<std::string> partialcitynames = partialCityName(queryCity);
    if (partialcitynames.size() == 1) {
        std::string temp = (*partialcitynames.begin());
        std::cout << "Autocomplete: " << temp << std::endl;
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("CityNameSearch"))), temp.c_str(), -1);
        queryCity = temp;
    }
    else {
        std::string tempOutput;
        tempOutput.append("Did you mean: \n");
        std::cout << "Possible cities: ";
        for (auto elem : partialcitynames) {
            std::cout << elem <<   ", ";
            tempOutput.append(elem);
            tempOutput.append(",\n");
        }
        std::cout << std::endl;
        
        GObject *window;
        GtkWidget *content_area;
        GtkWidget *label;
        GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
        GtkWidget *dialog;

        gtk_widget_set_size_request(scrolled_window, 150, 300);

        // GTK dialog window instantiation
        window = app->get_object(app->get_main_window_id().c_str());
        dialog = gtk_dialog_new_with_buttons("AutoSuggestions:", (GtkWindow*) window, GTK_DIALOG_DESTROY_WITH_PARENT, ("OK"), GTK_RESPONSE_ACCEPT, ("CANCEL"), GTK_RESPONSE_REJECT, NULL);
        content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        label = gtk_label_new(tempOutput.c_str());
        gtk_container_add (GTK_CONTAINER (scrolled_window), label);
        gtk_container_add(GTK_CONTAINER(content_area), scrolled_window);
        gtk_widget_show_all(scrolled_window);
        gtk_widget_show_all(dialog);
        g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(on_dialog_response), NULL);
        
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
    }
    load_map_from_map_name(queryCity);
    if(mapInputCorrect) {
        std::string loadingmessage = "Please wait, map is loading!";
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("CityNameSearch"))), queryCity.c_str(), -1);
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), loadingmessage.c_str(), -1);
        app->refresh_drawing();
    }
    
    if (mapInputCorrect) {
        if (mapLoaded){
            closeMap();
        }
        std::string temp = " loaded successfully!";
        std::string temp2 = queryCity;
        temp2.append(temp);
        std::cout << "Map: " << queryCity << " has been loaded successfully!" << std::endl;
        
        loadMap(tempFileName);
        loadIconPng(app);
        app->refresh_drawing();
        
        
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), temp2.c_str(), -1);
    }
    else if (partialcitynames.size() > 0) {
        std::cout << "Need more info, please provide the full name!" << std::endl;
        std::string temp = "Need more info!";
        //gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), temp.c_str(), -1);
    }
    else {
        std::string tempOutput  = "Invalid Input City";
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
    }

    app->refresh_drawing();
    return TRUE;
}

//utility callback function to clear the highlighted objects on the map (name is counter-intuitive, fix later)
gboolean press_intersection_finder(GtkWidget *, gpointer data) {
    auto app = static_cast<ezgl::application *>(data);
    for(auto it=findIntersections.begin();it!=findIntersections.end();it++) {
        intersectionInfoArray[*it].highlight = false;
    }
    findIntersections.clear();
    edge_col.clear();
    arrow_col.clear();
    startNode = -1;
    app->refresh_drawing();
             
    return TRUE;
}

//utility callback function to change the color scheme according to the mode toggle
gboolean change_mode(GtkWidget *, gpointer data) {
    auto app = static_cast<ezgl::application *>(data);
    lmode = !lmode;
    app->refresh_drawing();
    return TRUE;
}

//utility callback function to send an email with relevant location data when urgent help is requested via the emergency button
gboolean email_send(GtkWidget *, gpointer data) {
    std::cout<<"sending email"<<std::endl;
    
    if(longitude != -1 && latitude != -1){
        std::string lon = std::to_string(longitude);
        std::string lat = std::to_string(latitude);
        std::string msg = "Hi Amin, \n\n Lucas is near: "+  intName + 
                "\n\n The latitude & longitude of the closest intersection is: "
                + lon + "W " + lat+"N";

                //"amin1377.mohaghegh@gmail.com"
                // "lindy.zhai@mail.utoronto.ca"
        Email email({ "VIA.demo.ece297@gmail.com", "VIA Demo" },
                  "amin1377.mohaghegh@gmail.com",
                  "Hi from VIA!",
                  msg);

        email.send (  "smtp://smtp.gmail.com:25",
                      "VIA.demo.ece297@gmail.com",
                      "ece297.demo"  );
        longitude = -1;
        latitude = -1;
        intName = "";
    }
    return TRUE;
}


//utility callback function dealing with the autocomplete for the first search bar in particular
gboolean press_search_bar_1_button(GtkWidget *, gpointer data) {
    //clear previous intersections
    if (twoStreetsIntersections.size()>=1){
        for(auto inter:twoStreetsIntersections){
            intersectionInfoArray[inter].highlight = false;
        }
        twoStreetsIntersections.clear();
    }
    //partial street name
    auto app = static_cast<ezgl::application *>(data);

    std::string partialName = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("SearchBar1"))));
    
    if (partialName == "help") {
        gtk_widget_show(dialogHelp);
    }
    
    std::vector<StreetIdx> possibleStreets = findStreetIdsFromPartialStreetName(partialName);
    std::string tempOutput;
    if (possibleStreets.size() <= 0) {
        tempOutput  = "Invalid Input";
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
        searchBarStreet1=-1;
    }else if (possibleStreets.size() ==1){
        StreetIdx id = (*possibleStreets.begin());
        std::string name = getStreetName(id);
        if(partialName!=name){
            tempOutput="Autocomplete: " + name;
            std::cout << tempOutput << std::endl;
            gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("SearchBar1"))), name.c_str(), -1);
        }else{
            tempOutput=name;
        }
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
        searchBarStreet1=id;
        
        // if both streets are valid, highlight intersections
        bool haveIntersections=false;
        if (searchBarStreet1!=-1 && searchBarStreet2!=-1){
            for(auto inter: intersectionsOfStreet[searchBarStreet1]){
                if(intersectionsOfStreet[searchBarStreet2].find(inter)!=
                        intersectionsOfStreet[searchBarStreet2].end()){
                    twoStreetsIntersections.push_back(inter);
                    haveIntersections=true;
                }
            }
            if (haveIntersections){
                tempOutput="intersections highlighted";
            }else{
                tempOutput="no intersections";
            }
            gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(
                    app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
            
            for(auto inter:twoStreetsIntersections){
                intersectionInfoArray[inter].highlight = true;
            }
        }
        
    }else{
        std::cout<< "Multiple" << std::endl;
        tempOutput.append("Did you mean: \n");
        std::cout << "Possible streets: ";
        for (auto id : possibleStreets) {
            std::cout << getStreetName(id) <<   ", ";
            tempOutput.append(getStreetName(id));
            tempOutput.append(",\n");
        }
        std::cout << std::endl;
        
        GObject *window;
        GtkWidget *content_area;
        GtkWidget *label;
        GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
        GtkWidget *dialog;

        gtk_widget_set_size_request(scrolled_window, 150, 300);

        // GTK dialog window instantiation
        window = app->get_object(app->get_main_window_id().c_str());
        dialog = gtk_dialog_new_with_buttons("AutoSuggestions:", 
                (GtkWindow*) window, GTK_DIALOG_DESTROY_WITH_PARENT, 
                ("OK"), GTK_RESPONSE_ACCEPT, ("CANCEL"), 
                GTK_RESPONSE_REJECT, NULL);
        content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        label = gtk_label_new(tempOutput.c_str());
        gtk_container_add (GTK_CONTAINER (scrolled_window), label);
        gtk_container_add(GTK_CONTAINER(content_area), scrolled_window);
        gtk_widget_show_all(scrolled_window);
        gtk_widget_show_all(dialog);
        g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(on_dialog_response), NULL);
        
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
        searchBarStreet1=-1;
    }
    
    app->refresh_drawing();
    return TRUE;
}

//utility callback function dealing with the autocomplete for the second search bar in particular, also leading to drawing the intersection on the map
gboolean press_search_bar_2_button(GtkWidget *, gpointer data) {
    //clear previous intersections
    if (twoStreetsIntersections.size()>=1){
        for(auto inter:twoStreetsIntersections){
            intersectionInfoArray[inter].highlight = false;
        }
        twoStreetsIntersections.clear();
    }
    //partial street name
    auto app = static_cast<ezgl::application *>(data);

    std::string partialName = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("SearchBar2"))));
    if (partialName == "help") {
        gtk_widget_show(dialogHelp);
    }
    std::vector<StreetIdx> possibleStreets = findStreetIdsFromPartialStreetName(partialName);
    std::string tempOutput;
    if (possibleStreets.size() <= 0) {
        tempOutput  = "Invalid Input";
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
        searchBarStreet2=-1;
    }else if (possibleStreets.size() ==1){
        StreetIdx id = (*possibleStreets.begin());
        std::string name = getStreetName(id);
        if(partialName!=name){
            tempOutput="Autocomplete: " + name;
            std::cout << tempOutput << std::endl;
            gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("SearchBar2"))), name.c_str(), -1);
            gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
        }else{
            gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), name.c_str(), -1);
        }
        searchBarStreet2=id;
        
        // if both streets are valid, highlight intersections
        bool haveIntersections=false;
        if (searchBarStreet1!=-1 && searchBarStreet2!=-1){
            for(auto inter: intersectionsOfStreet[searchBarStreet1]){
                if(intersectionsOfStreet[searchBarStreet2].find(inter)!=
                        intersectionsOfStreet[searchBarStreet2].end()){
                    twoStreetsIntersections.push_back(inter);
                    haveIntersections=true;
                }
            }
            if (haveIntersections){
                tempOutput="intersections highlighted";
            }else{
                tempOutput="no intersections";
            }
            
            gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
            
            for(auto inter:twoStreetsIntersections){
                latitude = getIntersectionPosition(inter).latitude();
                longitude = getIntersectionPosition(inter).longitude();
                intName = getIntersectionName(inter);
                
//                std::cout<<"lol"<<std::endl;
                
                auto findinter = std::find(findIntersections.begin(), findIntersections.end(), inter);  
//                std::cout<<"lol"<<std::endl;
                if (findinter == findIntersections.end()){
                    findIntersections.push_back(inter);
                
                }else{
                    findIntersections.erase(findinter); 
                    findIntersections.push_back(inter);
                }
                    
//                std::cout<<"lol findinter: "<<*findinter<<std::endl;
//                findIntersections.push_back(inter);
                
                intersectionInfoArray[inter].highlight = true;
            }
        }
    }else{
        std::cout<< "Multiple" << std::endl;
        tempOutput.append("Did you mean: \n");
        std::cout << "Possible streets: ";
        for (auto id : possibleStreets) {
            std::cout << getStreetName(id) <<   ", ";
            tempOutput.append(getStreetName(id));
            tempOutput.append(",\n");
        }
        std::cout << std::endl;
        
        GObject *window;
        GtkWidget *content_area;
        GtkWidget *label;
        GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
        GtkWidget *dialog;

        gtk_widget_set_size_request(scrolled_window, 150, 300);

        // GTK dialog window instantiation
        window = app->get_object(app->get_main_window_id().c_str());
        dialog = gtk_dialog_new_with_buttons("AutoSuggestions:", (GtkWindow*) window, GTK_DIALOG_DESTROY_WITH_PARENT, ("OK"), GTK_RESPONSE_ACCEPT, ("CANCEL"), GTK_RESPONSE_REJECT, NULL);
        content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        label = gtk_label_new(tempOutput.c_str());
        gtk_container_add (GTK_CONTAINER (scrolled_window), label);
        gtk_container_add(GTK_CONTAINER(content_area), scrolled_window);
        gtk_widget_show_all(scrolled_window);
        gtk_widget_show_all(dialog);
        g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(on_dialog_response), NULL);
        
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(app->get_object("StatusSearch"))), tempOutput.c_str(), -1);
        searchBarStreet2=-1;
    }
       
    app->refresh_drawing();
    return TRUE;
}
  
//utility function to clear icons from the map
void clearIcon(){
    rendererP->free_surface(my_icon);
    rendererP->free_surface(pin);
    for(int i = 0; i< POIIcon.size(); i++){
        rendererP->free_surface(POIIcon[i]);
    }
    POIIcon.clear();
}

//utility function that draws the path between two intersections
std::vector<StreetSegmentIdx> drawPath(IntersectionIdx start, IntersectionIdx end) {
    std::vector<StreetSegmentIdx> array = findPathBetweenIntersections(0.0, std::make_pair(start, end));
    for(int i = 0; i < array.size();i++){
        for(int j = 0; j < streetsegEdge[array[i]].size();j++){
            simpleEdge edge = streetsegEdge[array[i]][j];
            // adding it to the edge_col global var so it will be drawn
            edge_col.push_back(edge);
        }
    }
    return array;
}

gboolean press_info_button(GtkWidget *, gpointer data){
    
    gtk_widget_show(dialogHelp);
            
    return TRUE;
}

//utility callback function for the popup info panel
//gboolean press_info_button(GtkWidget *, gpointer data) {
//    
//    auto application = static_cast<ezgl::application *>(data);
//
//    GObject *window;
//    GtkWidget *content_area;
//    
//    GtkWidget *dialog;
////    GtkWidget *widget;
////    GtkWidget * notebook = gtk_notebook_new();
////    GtkWidget * scroll1 = gtk_scrolled_window_new(NULL, NULL);
//    
//    window = application->get_object(application->get_main_window_id().c_str());
//    
//    dialog = gtk_dialog_new_with_buttons("VIA Help Center", (GtkWindow*) window, 
//            GTK_DIALOG_DESTROY_WITH_PARENT, ("OK"), 
//            GTK_RESPONSE_ACCEPT, ("CANCEL"), GTK_RESPONSE_REJECT, NULL);
//    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
//    gtk_widget_set_size_request(dialog, 500, 500);
//    gtk_container_add(GTK_CONTAINER(content_area), GTK_WIDGET(notebook));
//    gtk_widget_show_all(dialog);
////    gtk_widget_hide_on_delete(dialog);
//    g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(on_dialog_response), NULL);
//    
//    application->refresh_drawing();
//    
//    return TRUE;
//    
//}

//utility callback function that initiates pathfinding between all intersections highlighted  on the map (evaluated in order of highlighting)
gboolean press_navigation_button(GtkWidget *, gpointer data) {
    //std::vector<std::pair<IntersectionIdx, IntersectionIdx>> highlightedNodes;
    auto application = static_cast<ezgl::application *>(data);
    std::vector<std::vector<StreetSegmentIdx>> edges;
    std::vector<std::vector<std::string>> directions;
    std::string temp;
    edge_col.clear();
    arrow_col.clear();
    //auto app = static_cast<ezgl::application *>(data);
    double counter = 0;
    if(findIntersections.size() >= 2){
        auto it = findIntersections.begin();
        startNode = *it;
        IntersectionIdx firstN = startNode;
        IntersectionIdx secondN;
        it++;

        for (it;it!=findIntersections.end();it++) {
            secondN = *it;
            // append the edges and display then on the screen
            std::vector<StreetSegmentIdx> tempEdges = drawPath(firstN, secondN);
            edges.push_back(tempEdges);
            // create driving instructions for the streetsegs
            
            directions.push_back(getDirections(tempEdges, firstN)); 
            firstN = secondN;
            counter++;
        }
        
        for (double j = 0; j < counter; j++) {
            for (auto i = 0; i < directions[j].size(); i++) {
                std::cout << directions[j][i] << std::endl;
                temp.append(directions[j][i]);
            }
        }
        
        double time = 0.0;
        for(int sec = 0; sec < edges.size(); sec++){
            time += computePathTravelTime(turn_penalty,edges[sec]);
        }
        int hours = time/3600;
        int minute = (time-hours*3600)/60;
        int seconds = (time-minute*60-hours*3600);
        std::string timeString = "===========================================\n\n"
                "Driving time is approximately "+ std::to_string(hours)+ " hours, " 
                + std::to_string(minute) + " minutes, "+std::to_string(seconds)+"seconds.\n\n";
        std::cout<< "driving time is approximately "<< hours << " hours, " 
                << minute << " minutes, "<<seconds<<"seconds.\n\n";
        temp.append(timeString);
        std::cout << temp << std::endl;
        
        
        GObject *window;
        GtkWidget *content_area;
        GtkWidget *label;
        GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
        GtkWidget *dialog;

        gtk_widget_set_size_request(scrolled_window, 500, 500);

        // GTK dialog window instantiation
        window = application->get_object(application->get_main_window_id().c_str());
        dialog = gtk_dialog_new_with_buttons("Route Guidance - VIA Navigation", (GtkWindow*) window, GTK_DIALOG_DESTROY_WITH_PARENT, ("OK"), GTK_RESPONSE_ACCEPT, ("CANCEL"), GTK_RESPONSE_REJECT, NULL);
        content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
        label = gtk_label_new(temp.c_str());
        gtk_container_add (GTK_CONTAINER (scrolled_window), label);
        gtk_container_add(GTK_CONTAINER(content_area), scrolled_window);
        gtk_widget_show_all(scrolled_window);
        gtk_widget_show_all(dialog);
        g_signal_connect(GTK_DIALOG(dialog), "response", G_CALLBACK(on_dialog_response), NULL);    

    }
    else {
        std::string errorMessage  = "Invalid Input, please specify a destination";
        gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(application->get_object("StatusSearch"))), errorMessage.c_str(), -1);
    }
    counter = 0;
    application->refresh_drawing();
    return TRUE;
}

//utility function to determine the list of navigation directions from the calculated shortest path
std::vector<std::string> getDirections(std::vector<StreetSegmentIdx> segment, 
        IntersectionIdx sNode) {
    std::vector<std::string> turningInstructions;
    
//    double distance = 0;
    double totaldistance = 0;

    double numEdges = segment.size();
    double tempdist = findStreetSegmentLength(segment[0]);
    turningInstructions.push_back("===========================================\n");
    turningInstructions.push_back("Begin on " + 
        getStreetName(getStreetSegmentInfo(segment[0]).streetID) + 
        " and drive for " + std::to_string((int)round(tempdist))
        + " meters, and\n");
    
    totaldistance = tempdist;
    StreetSegmentInfo prevEdge = getStreetSegmentInfo(segment[0]);
    StreetIdx prevStreet = prevEdge.streetID;
    StreetIdx currStreet;
    IntersectionIdx fromN = prevEdge.from;
    IntersectionIdx toN = prevEdge.to;
    
    IntersectionIdx secondInt;
    
    if(fromN == sNode){
        secondInt = toN;
    }else{
        secondInt = fromN;
    }
    double rotateAngle;   
    double continuedis = 0;

    for (auto i = 1; i < numEdges; i++) {
        
        StreetSegmentInfo currEdge = getStreetSegmentInfo(segment[i]);
        currStreet = currEdge.streetID;
        fromN = currEdge.from;
        toN = currEdge.to;
        
        // rotation angle for the middle intersection arrows
        rotateAngle = AngleCalcIntersection(segment[i], currEdge, secondInt);
        arrow_col.push_back({secondInt, rotateAngle});
            //input node and rotation
//            arrow_col
        
        
        double theta = calculateAngleBetweenVectors(prevEdge, currEdge, segment[i-1], segment[i]);
        double pi = 3.141592653589;
        
        
        if (prevStreet == currStreet) {
            totaldistance += findStreetSegmentLength(segment[i]);
            continuedis += findStreetSegmentLength(segment[i]);

        }else {
            tempdist = findStreetSegmentLength(segment[i]);
            if(continuedis != 0){
                turningInstructions.push_back("Continue on " + 
            getStreetName(prevStreet) + " for an additional " + 
            std::to_string((int)round(continuedis)) 
            + " meters" + "\n");
                continuedis = 0;
            }
            
            auto itr = intersectionInfoArray[secondInt].street_col.begin();
            std::string object_str = *itr; itr++;
            while(itr != intersectionInfoArray[secondInt].street_col.end()){
                object_str += (" & " + *itr);
                itr++;
            }
            turningInstructions.push_back(
                "\nAt intersection " + object_str + ":");
            
            //left turns
            if ((theta > 0) && (theta < pi/4)) {turningInstructions.push_back(
                "\nSlight left onto " + getStreetName(currStreet) + "\n");}
            else if ((theta >= pi/4) && (theta < 3*pi/4)){turningInstructions.push_back(
                "\nTurn left onto " + getStreetName(currStreet) + "\n");}
            else if ((theta > 3*pi/4) && (theta < pi)) {turningInstructions.push_back(
                "\nSharp left onto " + getStreetName(currStreet) + "\n");}
            else if ((theta < 0) && (theta > -pi/4)) {turningInstructions.push_back(
            "\nSlight right onto " + getStreetName(currStreet) + "\n");}
            else if ((theta <= -pi/4) && (theta > -3*pi/4)) {turningInstructions.push_back(
            "\nTurn right onto " + getStreetName(currStreet) + "\n");}
            else if ((theta < -3*pi/4) && (theta > -pi)) {turningInstructions.push_back(
            "\nSharp right onto " + getStreetName(currStreet) + "\n");}
            
            totaldistance += tempdist;
            continuedis += tempdist;

        }
        
        if(fromN == secondInt){
            secondInt = toN;
        }else{
            secondInt = fromN;
        }
        
        prevEdge = currEdge;
        prevStreet = currStreet;
    }
    if(continuedis != 0){
        turningInstructions.push_back("Continue on " + 
    getStreetName(prevStreet) + " for an additional " + 
    std::to_string((int)round(continuedis)) 
    + " meters" + "\n");
        continuedis = 0;
    }
    
    turningInstructions.push_back("\nArrived at your destination!\n");
    turningInstructions.push_back("Total distance traveled on this route: approximately "
    + std::to_string((int) round(totaldistance)) + " meters\n\n");
    
    turningInstructions.push_back("\n\nEstimated taxi fee for highlighted route: " + std::to_string(baseTaxiPrice + perKmTaxiPrice * totaldistance/1000) + " " + priceUnits + ".\n\n");
    
    return turningInstructions;  
}

//utility function to calculate angle between intersections
double AngleCalcIntersection(StreetSegmentIdx edgeidx, StreetSegmentInfo edge, IntersectionIdx StartN){
    
    int numCurve = edge.numCurvePoints;
    IntersectionIdx NodeF = edge.from;
    IntersectionIdx NodeT; // the seg heading this direction
    
    if(NodeF == StartN){
        NodeT = edge.to;
    }else{
        NodeT = NodeF;
    }
    
    LatLon LLint = getIntersectionPosition(StartN);
    LatLon LLintN;
    
    if(numCurve!=0){
        if(NodeF == StartN){
            LLintN = getStreetSegmentCurvePoint(0, edgeidx);
        }else{
            LLintN = getStreetSegmentCurvePoint(numCurve-1, edgeidx);
        }
    }else{
        LLintN = getIntersectionPosition(NodeT);
    }
    
    double x1 = LontoX(LLint.longitude(), aveLat);
    double y1 = LatttoY(LLint.latitude());
    double x2 = LontoX(LLintN.longitude(), aveLat);
    double y2 = LatttoY(LLintN.latitude());
    
    return atan2(y2 - y1, x2 - x1)*180/PI;
    
}

//utility function to calculate angle between two vectors
double calculateAngleBetweenVectors(StreetSegmentInfo prevEdge, StreetSegmentInfo currEdge,
        StreetSegmentIdx previdx, StreetSegmentIdx curridx) {
    
    // grabing the last two nodes on a street (could be intersection if 
    // number of curvepoints is smaller than 2)
    IntersectionIdx prevToNode = prevEdge.to;
    
    int prevCurveNum = prevEdge.numCurvePoints;
    int currCurveNum = currEdge.numCurvePoints;

    LatLon prevLL1, prevLL2;
    
    // previous curvepoints
    
    if(prevCurveNum == 0){
        prevLL1 = getIntersectionPosition(prevEdge.from);
    }else{
        prevLL1 = getStreetSegmentCurvePoint(prevCurveNum - 1, previdx);
    }
    prevLL2 = getIntersectionPosition(prevToNode);    
    
    LatLon currLL1, currLL2;
    // current curvepoints
    if (prevEdge.to == currEdge.from) {
        currLL1 = getIntersectionPosition(currEdge.from);
        if(currCurveNum == 0){
            currLL2 = getIntersectionPosition(currEdge.to);
        }else{
            currLL2 = getStreetSegmentCurvePoint(0, curridx);
        }
    }
    else{
        currLL1 = getIntersectionPosition(currEdge.to);
        if(currCurveNum == 0){
            currLL2 = getIntersectionPosition(currEdge.from);
        }else{
            currLL2 = getStreetSegmentCurvePoint(currCurveNum-1, curridx);
        }
    }
    
    // converting previous and current latlon points into x, y coords
    
    double Prevx1 = LontoX(prevLL1.longitude(), aveLat);
    double Prevy1 = LatttoY(prevLL1.latitude());
    double Prevx2 = LontoX(prevLL2.longitude(), aveLat);
    double Prevy2 = LatttoY(prevLL2.latitude());
    
    double Currx1 = LontoX(currLL1.longitude(), aveLat);
    double Curry1 = LatttoY(currLL1.latitude());
    double Currx2 = LontoX(currLL2.longitude(), aveLat);
    double Curry2 = LatttoY(currLL2.latitude());
    
    // creating direction vectors from the previous and current streetsegments
    std::pair prevVector = std::make_pair(Prevy2-Prevy1, Prevx2-Prevx1);
    std::pair currVector = std::make_pair(Curry2-Curry1, Currx2-Currx1); 
    
    double dotProduct = (prevVector.second * currVector.second) 
        + ( prevVector.first* currVector.first);
    double determinant = (prevVector.second * currVector.first) 
        - ( prevVector.first* currVector.second);
    
//    std::cout<<"the output angle is: "<<atan2(determinant, dotProduct)/3.1415926*180.0<<std::endl;
    return atan2(determinant, dotProduct);
}