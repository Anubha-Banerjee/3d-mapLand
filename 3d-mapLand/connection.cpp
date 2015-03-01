#include "connection.h"
#include <cstdio>
#include <string>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include "map.h"


int connect_and_save(string query, FILE *fp)
{
	// clear the file before saving new content
	fp = fopen ("data\\data.txt", "w+");
//	const std::string query = "GET /cgi/interpreter?data=[out:json];(node(18.51507,73.87276,18.53597,73.90142);way(18.51507,73.87276,18.53597,73.90142);node(w)->.x;);out; HTTP/1.1\r\n"
	//						  "Host: overpass.osm.rambler.ru\r\n"
		//					  "User-Agent: anubha(BE student making 'drive on earth' simulation project)\r\n"
			//				  "Accept: */*\r\n"
				//			  "Connection: close\r\n"
					//		  "\r\n";


	// Initialize Winsock.

	cout << "connecting1\n";
	WSADATA wsadata;
	int iResult = WSAStartup (MAKEWORD(2,2), &wsadata );
	if (iResult !=NO_ERROR )
		printf("\nmyERROR at WSAStartup()\n");
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("error opening socket"); return -1;
    }

	struct hostent *remoteHost;
	struct in_addr addr;
	remoteHost = gethostbyname(host.c_str());
	addr.s_addr = *(u_long *)remoteHost->h_addr_list[0];

    struct sockaddr_in sin;
    sin.sin_port = htons(port);
	sin.sin_addr.s_addr = inet_addr(inet_ntoa(addr));
    sin.sin_family = AF_INET;

	char hostname[NI_MAXHOST];
	char servInfo[NI_MAXSERV];

    if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
        perror("error connecting to host"); return -1;
    }
    const int query_len = query.length() + 1; // trailing '\0'
    if (send(sock, query.c_str(), query_len, 0) != query_len) {
        perror("error sending query"); return -1;
    }

    const int buf_size = 1024 * 1024;
    while (true) {
        std::vector<char> buf(buf_size, '\0');
        const int recv_len = recv(sock, &buf[0], buf_size - 1, 0);

        if (recv_len == -1) {
            perror("error receiving response"); return -1;
        } else if (recv_len == 0) {
            std::cout << std::endl; break;
        } else {
            std::cout << &buf[0];			
			fprintf(fp, "%s", &buf[0]);
        }
    }
	fseek(fp, 0, SEEK_SET);
}

// takes in lat and lon and returns the query that should be sent to server
string createQuery(double lat, double lon)
{
	string query;
	double lat2 = lat + width;
	double lon2 = lon + length;

	std::string str_lat = std::to_string(lat);
	std::string str_lon = std::to_string(lon);

	std::string str_lat2 = std::to_string(lat2);
	std::string str_lon2 = std::to_string(lon2);
	
	string comma = ",";
	string queryStart = "GET /cgi/interpreter?data=[out:json];(node(" ;
	string queryEnd = ");node(w)->.x;);out; HTTP/1.1\r\n"
			        "Host: overpass.osm.rambler.ru\r\n"
					"User-Agent: anubha(BE student making 'drive on earth' simulation project)\r\n"
					"Accept: */*\r\n"
					"Connection: close\r\n"
					"\r\n";

	string query1 = str_lat;
	string query2 = str_lon;
	string query3 = str_lat2;
	string query4 = str_lon2;
	string way = ");way(";

	string box = query1 + comma + query2 + comma + query3 + comma + query4;	
	query = queryStart + box + way + box + queryEnd;
	cout << query;		
	return query;
}