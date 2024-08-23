#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <cmath>
#include <climits>
#include <unordered_map>
#include <set>
#include <algorithm>
#include "rapidxml/rapidxml_utils.hpp"

#define PI 3.14159265
#define INF 0x3f3f3f3f
using namespace std;

/*
Name: Jyoti Kamal
Software Engineering Assignment 
*/

// Library used: RapidXML
// RapidXML : http://rapidxml.sourceforge.net/

static double haversine(double lat1, double lon1, double lat2, double lon2)
{
        // distance between latitudes
        // and longitudes
        double dLat = (lat2 - lat1) * PI / 180.0;
        double dLon = (lon2 - lon1) * PI / 180.0;
 
        // convert to radians
        lat1 = (lat1) * PI / 180.0;
        lat2 = (lat2) * PI / 180.0;
 
        // apply formulae
        double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2);
        // rad is Earth's radius in KM
        double rad = 6372797.56085;
        double c = 2 * asin(sqrt(a));
        return rad * c;
}

void countNodesWays(rapidxml::xml_node<> *root, int &nodes, int &ways)
{
    nodes = 0;
    ways = 0;

    for(root = root->first_node(); root != nullptr; root = root->next_sibling())
    {
        if(strcmp(root->name(), "node") == 0) nodes++;
        else if(strcmp(root->name(), "way") == 0) ways++;
    }
}

int isSubstring(string s1, string s2)
{
    int m = s1.length();
    int n = s2.length();

    for (int i = 0; i <= n - m; i++) 
    {
        int j;
        for (j = 0; j < m; j++)
            if (s2[i + j] != s1[j]) break;

        if (j == m) return i;
    }
    return -1;
}


void searchElement(rapidxml::xml_node<> *root, string tag)
{
    transform(tag.begin(), tag.end(), tag.begin(), ::tolower);   
    int f = 0;
    cout << "Initiating a search\n\n";
    for(rapidxml::xml_node<> *child = root->first_node(); child; child = child->next_sibling())
    {
        rapidxml::xml_node<> *refnext = child->first_node("tag");
        if(!refnext) continue;
        // cout << "haha\n";
        rapidxml::xml_attribute<> *attr = refnext->first_attribute("k");
        if(!attr) continue;
        // cout << attr->value();
        while (strcmp(attr->value(), "name"))
        {
            refnext = refnext->next_sibling();
            if(!refnext) break;
            if(strcmp(refnext->name(), "tag")) continue;
            attr = refnext->first_attribute("k");
        }
        if(strcmp(attr->value(), "name")) continue;
        string potentialName = refnext->first_attribute("v")->value();
        // cout << " " << potentialName << "\n";
        transform(potentialName.begin(), potentialName.end(), potentialName.begin(), ::tolower); 
        if(isSubstring(tag, potentialName) > -1) 
        {
            cout << "\t[ " << child->name() << " ] "<< potentialName << ", ID : " << child->first_attribute("id")->value() << "\n";
            f++;
        }
    }
    if(!f) cout << "There were no elements found\n\n";
    else cout << "\nFound total " << f << " elements\n";
}

unordered_map<string, int> parseElements(rapidxml::xml_node<> *root)
{
    unordered_map<string, int> map;

    for(rapidxml::xml_node<> *child = root->first_node(); child; child = child->next_sibling())
    {
        rapidxml::xml_node<> *temp = child->first_node();
        string t = child->name();
        map[t]++;
    }
    return map;
}

void kNearestNodes(rapidxml::xml_node<> *root, rapidxml::xml_node<> *node, int k)
{
    vector<rapidxml::xml_node<> *> v(k, nullptr);
    unordered_map<string, bool> present;

    string id = node->first_attribute("id")->value();
    present[id] = true;

    for(int i = 0; i < k; i++)
    {
        int distance = INT_MAX;
        rapidxml::xml_node<>* currmin = nullptr;

        for(rapidxml::xml_node<> *child = root->first_node(); child; child = child->next_sibling())
        {
            if(strcmp(child->name(), "node")) continue;

            rapidxml::xml_attribute<> *attr = child->first_attribute("id");
            
            if(!attr) continue;

            string other_id;
            other_id = attr->value();

            if(present.find(other_id) != present.end()) continue;

            else 
            {
               double temp = haversine(atof(child->first_attribute("lat")->value()),
                    atof(child->first_attribute("lon")->value()),
                    atof(node->first_attribute("lat")->value()), 
                    atof(node->first_attribute("lon")->value()));

                if(temp < distance) 
                {
                    distance = temp;
                    currmin = child;
                }
            }
        }
        if(!currmin) cout << "Something went wrong\n";
        present[currmin->first_attribute("id")->value()] = true;
        v[i] = currmin;
    }
    cout << "\nFound the following " << k << " nodes nearest to it:\n\n";
    for(auto y : v)
    {
        cout << "ID : " << y->first_attribute("id")->value() << "\n";
        cout << "\tlat : " << y->first_attribute("lat")->value();
        cout << "\tlon : " << y->first_attribute("lon")->value();
        cout << "\n\n";
    }
}

rapidxml::xml_node<>* findRefNode(rapidxml::xml_node<> *root, string ref)
{
    rapidxml::xml_node<> *child;
    rapidxml::xml_node<> *found = nullptr;

    for(child = root->first_node(); child; child = child->next_sibling())
    {
        if(!child->first_attribute("id")) continue;
        if(child->first_attribute("id")->value() == ref)
        {
            found = child;  
            break;
        }
    }
    return found;
}

unordered_map<string, rapidxml::xml_node<>*> parseByID(rapidxml::xml_node<> * root)
{
    unordered_map<string, rapidxml::xml_node<>* > map;
    for(rapidxml::xml_node<> *child = root->first_node("node"); child; child = child->next_sibling())
    {
        if(strcmp(child->name(), "node") != 0) continue;
        rapidxml::xml_attribute<> *attr = child->first_attribute("id");

        if(!attr) continue;
        map.insert({attr->value(), child});
    }
    return map;
}

unordered_map<string, vector<string>> getGraph(rapidxml::xml_node<> *root)
{
    unordered_map<string, vector<string>> map;

    for(rapidxml::xml_node<> *child = root->first_node("way"); child; child = child->next_sibling())
    {
        if(strcmp(child->name(), "way")) continue;
        for(rapidxml::xml_node<> *ref = child->first_node("nd"); ref->next_sibling(); ref = ref->next_sibling())
        {
            rapidxml::xml_node<> *refnext = ref->next_sibling();

            if(strcmp(ref->name(), "nd") || strcmp(refnext->name(), "nd")) continue;
            rapidxml::xml_attribute<> *attr = ref->first_attribute("ref");

            //Allocate memory if not done
            if(map.find(attr->value()) == map.end()) map[attr->value()] = *(new vector<string>);
            if(map.find(refnext->first_attribute("ref")->value()) == map.end()) map[refnext->first_attribute("ref")->value()] = *(new vector<string>);
            
            //Assign adjacent vertices
            map[attr->value()].push_back(refnext->first_attribute("ref")->value());
            map[refnext->first_attribute("ref")->value()].push_back(attr->value());
        }
    }
    return map;
}

void shortestPath(rapidxml::xml_node<> * root, rapidxml::xml_node<> * node1, rapidxml::xml_node<> * node2, unordered_map<string, vector<string>> mapAdj, unordered_map<string, rapidxml::xml_node<> *> mapID)
{
    //wt and vertex
    set< pair<double, string> > setds;

    //vertex and its distance from node1
    unordered_map<string, double> dist;

    for(auto i : mapID)
        dist[i.first] = INF;

    setds.insert(make_pair(0, node1->first_attribute("id")->value()));
    dist[node1->first_attribute("id")->value()] = 0;

    while (!setds.empty())
    {
        // The first vertex in Set is the minimum distance
        pair<double, string> tmp = *(setds.begin());

        //Current node
        string u = tmp.second;

        //Break if the current node is itself the destination
        if(u == node2->first_attribute("id")->value()) break;
        setds.erase(setds.begin());

        vector<string>::iterator i;
        for (i = mapAdj[u].begin(); i != mapAdj[u].end(); ++i)
        {
            //Neighboring node
            string v = (*i);
            
            double weight = haversine(stod(mapID[u]->first_attribute("lat")->value()), 
            stod(mapID[u]->first_attribute("lon")->value()), 
            stod(mapID[v]->first_attribute("lat")->value()),
            stod(mapID[v]->first_attribute("lon")->value()));

            //If there is shorter path to v through u.
            if (dist[v] > dist[u] + weight)
            {
                if (dist[v] != INF)
                    setds.erase(setds.find(make_pair(dist[v], v)));

                dist[v] = dist[u] + weight;
                setds.insert(make_pair(dist[v], v));
            }
        }
    }
    
    // Print shortest distances stored in dist[]
    if(dist[node2->first_attribute("id")->value()] == INF) cout << "No path found\n";
    else 
    {
        cout << "Found the shortest path: ";
        cout << dist[node2->first_attribute("id")->value()] << " meters\n\n";
    }
}

int main()
{
    rapidxml::file<> xmlFile("aligarh_map.osm");
    rapidxml::xml_document<> doc;
    doc.parse<0>(xmlFile.data());

    rapidxml::xml_node<> *root = doc.first_node("osm");

    unordered_map<string, int> mapByString = parseElements(root);
    unordered_map<string, rapidxml::xml_node<>*> mapByID = parseByID(root);
    unordered_map<string, vector<string>> adjMap = getGraph(root);

    int op = -1;
    while (op != 4)
    {
        cout << "Select one option [1 - 4]\n";
        cout << "1 - Search for an element in the osm\n";
        cout << "2 - k closest node\n";
        cout << "3 - Shortest path between two nodes\n";
        cout << "4 - Exit program\n";
        cout << ":";
        cin >> op;

        switch(op)
        {
            case 1:
            {
                cout << "Number of nodes: " << mapByString["node"] << "\n";
                cout << "Number of ways: " << mapByString["way"] << "\n\n";
                cout << "Input string: ";
                string tag;
                cin >> tag;
                searchElement(root, tag);
                break;
            }
            case 2:
            {
                string ref;
                cout << "Input reference for node: ";
                cin >> ref;
                int k;
                cout << "Input k: ";
                cin >> k;

                rapidxml::xml_node<> * child;
                rapidxml::xml_node<> * found = nullptr;

                found = findRefNode(root, ref);
                if(found && k > 0) 
                {
                    cout << "Found reference:\n\t" << found->name() << " : " << found->first_attribute("id")->value() << "\n";
                    cout << "\tlatitude : " << found->first_attribute("lat")->value() << "\n";
                    cout << "\tlongitude : " << found->first_attribute("lon")->value() << "\n"; 

                    kNearestNodes(root, found, k);
                }   

                else cout << "Reference not found or invalid value of k, try again\n\n";
                break;             
            }
            case 3:
            {
                string ref1, ref2;
                cout << "Input reference for first node: ";
                cin >> ref1;
                cout << "Input reference for second node: ";
                cin >> ref2;
                rapidxml::xml_node<> * found1 = findRefNode(root, ref1);
                rapidxml::xml_node<> * found2 = findRefNode(root, ref2);
                if(!found1 || strcmp(found1->name(), "node"))
                {
                    cout << "\nThere is no node with this given ID\n\n";
                    continue;
                } else if (!found2 || strcmp(found2->name(), "node"))
                {
                    cout << "\nThere is no node with this given ID\n\n";
                    continue;
                }
                cout << "neighbours 1: \n";
                for(auto j : adjMap[ref1]) cout << j << "\n";
                cout << "\n";
                cout << "neighbours 2: \n";

                for(auto j : adjMap[ref2]) cout << j << "\n";
                cout << "\n";
                shortestPath(root, found1, found2, adjMap, mapByID);
                break;
            }
            case 4:
            {
                cout << "Thankyou for using this program\n";
                cout << "\tBye!\n";
                break;
            }
            default:
            {
                cout << "Please input a valid number [1 - 4]\n";
                break;
            }
        }
    }
    return 0;
}
