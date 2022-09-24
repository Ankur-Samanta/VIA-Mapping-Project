

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
#include <sstream>
#include <string>
#include <ctime>
#include <algorithm>
#include <random>
#include "m1.h"
#include "m2.h"
#include "m2Helper/tileMapConfig.h"
#include "email_sender.h"
#include <curl/curl.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
 
/*
 * For an SMTP example using the multi interface please see smtp-multi.c.
 */
 
/* The libcurl options want plain addresses, the viewable headers in the mail
 * can very well get a full name as well.
 */

//Program exit codes
constexpr int SUCCESS_EXIT_CODE = 0;        //Everyting went OK
constexpr int ERROR_EXIT_CODE = 1;          //An error occured
constexpr int BAD_ARGUMENTS_EXIT_CODE = 2;  //Invalid command-line usage

std::string default_map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";

int main(int argc, char** argv) {
    
    

    std::string map_path;
    if(argc == 1) {
        //Use a default map
        map_path = default_map_path;
    } else if (argc == 2) {
        //Get the map from the command line
        map_path = argv[1];
    } else {
        //Invalid arguments
        std::cerr << "Usage: " << argv[0] << " [map_file_path]\n";
        std::cerr << "  If no map_file_path is provided a default map is loaded.\n";
        return BAD_ARGUMENTS_EXIT_CODE;
    }

    //Load the map and related data structures
    mapLoaded = loadMap(map_path);
    if(!mapLoaded) {
        std::cerr << "Failed to load map " << map_path << "'\n";
        return ERROR_EXIT_CODE;
    }
    
//    loadMap("/cad2/ece297s/public/maps/london_england.streets.bin");// tokyo_japan
    
    drawMap();
    
    closeMap(); //To be removed. 

    return SUCCESS_EXIT_CODE;
}
