#include "m1.h"
#include "m3.h"
#include "loadM3.h"
#include "m2Helper/intersectionStreetFunc.h"
#define INFINITETIME = -1
#define NOPREV = -1

#include <list>
#include <vector>
#include "tileMapConfig.h"

//function declarations
double calculatecost(double accumulateT, IntersectionIdx a, IntersectionIdx b);
double calculateHHeuristic(IntersectionIdx a, IntersectionIdx b);


//computes the travel time of a path given the turn penalty and a vector of street segments defining the path
double computePathTravelTime(const double turn_penalty,
        const std::vector<StreetSegmentIdx>& path){
    int numseg = path.size();
    double totaltime = 0.0; 
    StreetSegmentIdx idx1 = path[0];
    totaltime +=  streetSegVec[idx1].travelTime;
    if(numseg>1){
        StreetIdx id1 = streetSegVec[idx1].streetID;
        StreetSegmentIdx idx2;
        for(int i = 1; i<numseg-1; i++){
            idx2 = path[i];
            StreetIdx id2 = streetSegVec[idx2].streetID;
            if(id1 != id2){
                totaltime += turn_penalty;
            }
            totaltime +=  streetSegVec[idx2].travelTime;
            id1 = id2;
        }
        if(id1!=streetSegVec[path[numseg-1]].streetID){
            totaltime += turn_penalty;
        }
        totaltime +=  streetSegVec[path[numseg-1]].travelTime;
    }
    return totaltime;
}

//Uses A* to compute the shortest path between 2 given intersections
std::vector<StreetSegmentIdx> findPathBetweenIntersections(
const double turn_penalty,
const std::pair<IntersectionIdx, IntersectionIdx> intersect_ids){
    //start and end intersections
    IntersectionIdx start = intersect_ids.first;
    IntersectionIdx dest = intersect_ids.second;
    
    //variable definitions
    double actual_time;
    
    // set up priority queue; see header file for comparator function definition
    std::priority_queue<WaveElem, std::vector<WaveElem>, 
            CompareTime> PQ;
    
    // instantiate new map, we use a hash table here
    std::unordered_map<IntersectionIdx, Node> nodeHash;
    // insert first node
    nodeHash.insert({start, Node(0.0, -1, -1)});
    
    // iterate through all the edges for the first node as defined in the adjacency list
    for(auto i : adjacency_list[start]) {
        // pull the actual travel time up to that point as stored in streetSegVec (See loading structure in loadM3 for details)
        actual_time = streetSegVec[i.second].travelTime;
        // create new node with updated information
        Node newN = Node(actual_time, i.second, start);
        // inserting new element into hashtable
        nodeHash.insert(std::make_pair(i.first, newN));
        // create element to be inserted into the priority queue
        WaveElem pqele = WaveElem(calculatecost(actual_time,i.first,dest)
                , i.first, i.second);

        // inserting element into the priority queue
        PQ.push(pqele);
    }
    
    // while the priority queue is empty
    while(!PQ.empty()){
        // get the element at the top of the priority queue (remember this is already sorted as per the defined heuristic
        WaveElem ele = PQ.top();
        
        // remove it from priority queue
        PQ.pop();
        IntersectionIdx nodeCheck = ele.curNode;
        // check if destination has been reached, and if so generate the return path
        if(nodeCheck == dest){
            std::vector<StreetSegmentIdx> returnPath;
            IntersectionIdx currentNode = nodeCheck;
            while (currentNode!=start){
                auto getNode = nodeHash.find(currentNode);
                returnPath.push_back(getNode->second.prevPath);
                currentNode=getNode->second.prevNode;
            }
            std::reverse(returnPath.begin(), returnPath.end());
            return returnPath;
        }
        //otherwise, carry on
        else{
            auto getNode = nodeHash.find(nodeCheck);
            actual_time = getNode->second.time;
            StreetIdx pastedgeid;
            if(getNode->second.prevPath != -1){
                pastedgeid = streetSegVec[getNode->second.prevPath].streetID;
            }else{
                pastedgeid = -1;
            }
            
            // iterate through all the edges for a given node as defined in the adjacency list
            for(auto i : adjacency_list[nodeCheck]) {
//                if(adjacency_list[nodeCheck].size())
                //get the edge id
                StreetIdx edgeid = streetSegVec[i.second].streetID;
                //calculate the accumulated time to be used in heuristic calculation
                double accumulateT = actual_time + streetSegVec[i.second].travelTime;
                //account for the turn penalty
                if((pastedgeid != edgeid)&&(pastedgeid != -1)){
                    accumulateT += turn_penalty;
                }
                getNode = nodeHash.find(i.first);
                if(getNode == nodeHash.end()){
                    Node newN = Node(accumulateT, i.second, nodeCheck);
                    nodeHash.insert(std::make_pair(i.first, newN));
                    WaveElem pqele = WaveElem(
                            calculatecost(accumulateT,i.first,dest), 
                            i.first, i.second);
                    PQ.push(pqele);
                    
                }else{
                    double pastTime = getNode->second.time;
                    if(pastTime>accumulateT){
                        Node newN = Node(accumulateT, i.second, nodeCheck);
                        getNode->second = newN;
                        WaveElem pqele = WaveElem(
                                calculatecost(accumulateT,i.first,dest),
                                i.first, i.second);

                        PQ.push(pqele);
                    }
                }
            }
        }
    }
    //otherwise default to returning an empty vector
    std::vector<StreetSegmentIdx> empty;
    return empty;
}

//utility function to calculate the h heuristic
double calculateHHeuristic(IntersectionIdx a, IntersectionIdx b) {
    LatLon alatlon = getIntersectionPosition(a);
    LatLon blatlon = getIntersectionPosition(b);
    return findDistanceBetweenTwoPoints(std::make_pair(alatlon, blatlon))/globalSpeedlim;
}

//utility function to finish calculating the cost based off of the heuristic calculation
double calculatecost(double accumulateT, IntersectionIdx a, IntersectionIdx b){
    return calculateHHeuristic(a, b) + accumulateT;
}
