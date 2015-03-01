#include <iostream>

using namespace std; 

const std::string host = "overpass.osm.rambler.ru";
const int port = 80;
#define NI_MAXSERV    32
#define NI_MAXHOST  1025

int connect_and_save(string query, FILE *fp);
string createQuery(double lat, double lon);