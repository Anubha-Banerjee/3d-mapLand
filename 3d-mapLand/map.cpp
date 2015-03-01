#include <iostream>
#include <math3d.h>
#include "map.h"
#include "model.h"
#include <GLTools.h>
#include <sstream>

using namespace std;

// returns 1 if the user wishes to load data from net
bool Map::inputCoordinates()
{
	int choice;
	cout << "\nEnter the choice\n1.provide coordinates and load data from net \n2.Use some preloaded locations\n";
	cin >> choice;


	if (choice == 1)
	{

		cout << "Enter the latitude : ";
		cin >> map_latitude;

		cout << "Enter the longitude : ";
		cin >> map_longitude;

		strcpy(file, "data\\data.txt");

		return 1;
	}

	else
	{
		cout << "\nenter the choice of location : \n1. mg road pune\n2. kolkata\n3. near mumbai race course\n4. in germany \n5. in china\n";
		cin >> choice;

		switch (choice)
		{
			// mg road
		case 1:
			map_latitude = 18.51507;
			map_longitude = 73.87276;
			strcpy(file, "data\\mg road data.txt");
			break;

			// kata
		case 2:
			map_latitude = 22.572646;
			map_longitude = 88.363895;
			strcpy(file, "data\\kolkata.txt");
			break;

			// mumbai
		case 3:
			map_latitude = 18.92235;
			map_longitude = 72.82898;
			strcpy(file, "data\\near mumbai racecourse.txt");
			break;

			// germany
		case 4:
			map_latitude = 50.746;
			map_longitude = 7.154;
			strcpy(file, "data\\germany data.txt");
			break;

			// china
		case 5:
			map_latitude = 31.2321150;
			map_longitude = 121.4917826;
			strcpy(file, "data\\china data.txt");
			break;
		}
	}
	return 0;
}


void Map::calculateShift(double x, double y)
{
	int w = 2000;
	double latRad = m3dDegToRad(x + width / 2);
	double lonRad = m3dDegToRad(y + length / 2);

	// apply the mercator projection to the top left corner of the bounding box, so we can shift the whole map on to the screen
	shiftX = -(((w / (2 * M_PI)) * (lonRad)* SCALE));
	shiftY = -((w / (2 * M_PI) * log(tan(M_PI / 4 + latRad / 2)) * SCALE));
}

bool readWhiteSpaces(FILE *fp)
{

	char ch;
	bool word_break = true;

	// eat ch if its newline
	while (true)
	{
		fscanf(fp, "%c", &ch);
		if (ch == '\n')
		{
			word_break = false;
		}
		else
		{
			// go one back as loop will have to break as character not \n'
			fseek(fp, -1, SEEK_CUR);
			break;
		}
	}

	// eat ch if its space
	while (true)
	{
		fscanf(fp, "%c", &ch);
		if (ch == ' ')
		{
			word_break = false;
		}
		else
		{
			// go one back as loop will have to break as character not \n'
			fseek(fp, -1, SEEK_CUR);
			break;
		}
	}
	return word_break;
}

void readWord(FILE *fp, char word[50])
{
	char ch;
	char chunkSize[50], wordEnd[50];
	bool word_break = false;

	fscanf(fp, "%s", word);

	// eat whitespaces
	word_break = readWhiteSpaces(fp);

	// read the next char
	fscanf(fp, "%c", &ch);
	if (ch == '\r')
	{
		fscanf(fp, "%s", chunkSize);
		//cout << "\nchunk size : " << chunkSize;

		// read off \r and \n following the chunk size
		fscanf(fp, "%c", &ch);
		fscanf(fp, "%c", &ch);

		if (!strcmp(chunkSize, "0"))
		{
			cout << "\nend of input";
			strcpy(word, chunkSize);
			return;
		}

		// as if word break is false then it is seen that word is not broken by chunk size
		if (word_break == true)
			word_break = readWhiteSpaces(fp);

		if (word_break)
		{
			fscanf(fp, "%s", wordEnd);
			strcat(word, wordEnd);
			//cout << "\nFull word: " << word;
		}
	}
	else
		fseek(fp, -1, SEEK_CUR);

}

double convert_lat_to_y(double lat)
{
	double y = 0, w = 2000;
	double latRad = m3dDegToRad(lat);
	y = (w / (2 * M_PI) * log(tan(M_PI / 4 + latRad / 2)) * SCALE);
	y = y + map.shiftY;
	return y;
}

double convert_lon_to_x(double lon)
{
	double x = 0, w = 2000;
	float lonRad = m3dDegToRad(lon);
	//float ylonRad =  m3dDegToRad(90);

	x = ((w / (2 * M_PI)) * (lonRad)* SCALE);
	x = -(x + map.shiftX);

	return x;
}

void storeTagInArray(nodeTag *array, int *index, node curr_node)
{
	strcpy(array[*index].name, "empty");
	array[*index].x = curr_node.x;
	array[*index].y = curr_node.y;


	*index = *index + 1;
}

char* returnName(FILE *fp)
{
	char word[1000], amenityName[1000] = "empty";
	while (strcmp(word, "\"name\":"))
	{
		readWord(fp, word);
		if (!strcmp(word, "}"))
		{
			return amenityName;
		}
	}

	// the name of the amenity
	char ch = 'a';
	int i = 0;

	// read " at begining of name
	fscanf(fp, "%c", &ch);
	while (ch != '\n')
	{
		fscanf(fp, "%c", &ch);
		amenityName[i] = ch;
		i++;
	}

	// so that ending " is not stored
	if (amenityName[i - 2] == ',')
		amenityName[i - 3] = NULL;
	else amenityName[i - 2] = NULL;

	//cout << "\n" << amenityName;
	return amenityName;
}

// this function stores the nodes from file fp and stores in array of nodes
void Map::store_nodes_in_array(FILE *fp)
{
	cout << "\n\nstoring nodes in ways...";

	map.nodes = new node[MAX_NODES];
	map.trafficLights = new nodeTag[MAX_NODE_TAGS];
	roundabouts = new nodeTag[MAX_NODE_TAGS];
	eatingPlaces = new nodeTag[MAX_NODE_TAGS];
	hospitals = new nodeTag[MAX_NODE_TAGS];
	parkings = new nodeTag[MAX_NODE_TAGS];
	busStops = new nodeTag[MAX_NODE_TAGS];
	oneways = new nodeTag[MAX_NODE_TAGS];
	trees = new nodeTag[MAX_NODE_TAGS];

	//map.hotels = new hotel[MAX_HOTELS];

	char word[1000];
	double lat, lon;
	int treeRand;


	fseek(fp, 0, SEEK_SET);
	while (!feof(fp))
	{
		readWord(fp, word);

		// break off if its last chunk size 0
		if (!strcmp(word, "0"))
			break;

		// break off if word is way
		if (!strcmp(word, "\"way\","))
		{
			fseek(fp, -10, SEEK_CUR);
			break;
		}

		if (!strcmp(word, "\"node\","))
		{

			// a random no. to see if I draw a tree
			treeRand = rand() % 1000;


			// read "id":
			readWord(fp, word);


			// read the acutal node id value
			readWord(fp, word);

			// converting string to long long
			std::stringstream sstr(word);
			__int64 val;
			sstr >> val;

			nodes[nodeCount].id = val;

			// read "lat:"
			readWord(fp, word);

			// read latitude value
			readWord(fp, word);

			lat = atof(word);
			nodes[nodeCount].y = convert_lat_to_y(lat);

			// read "lon:"
			readWord(fp, word);

			// read longitude value
			readWord(fp, word);
			lon = atof(word);
			nodes[nodeCount].x = convert_lon_to_x(lon);


			// now take care of the tags in a node
			readWord(fp, word);

			// 50/1000 probability of a tree.
			if (treeRand < 50 && treeCount < MAX_NODE_TAGS)
			{
				storeTagInArray(trees, &treeCount, nodes[nodeCount]);
			}

			if (!strcmp(word, "\"tags\":"))
			{
				// read the tags till you encounter some street element
				while (strcmp(word, "},"))
				{
					readWord(fp, word);

					if (!strcmp(word, "\"traffic_signals\",") || !strcmp(word, "\"traffic_signals\""))
					{
						storeTagInArray(trafficLights, &trafficLightCount, nodes[nodeCount]);
						if (trafficLightCount  > MAX_NODE_TAGS)
						{
							cout << "too much tl ";
							exit(0);
						}
					}
					if (!strcmp(word, "\"mini_roundabout\",") || !strcmp(word, "\"mini_roundabout\""))
					{
						//cout << "\nA roundabouttttt";
						storeTagInArray(roundabouts, &roundaboutCount, nodes[nodeCount]);
					}
					if (!strcmp(word, "\"restaurant\",") || !strcmp(word, "\"restaurant\"") || !strcmp(word, "\"brand\":"))
					{
						char *hotelName;
						storeTagInArray(eatingPlaces, &eatingPlaceCount, nodes[nodeCount]);
						hotelName = returnName(fp);
						strcpy(eatingPlaces[eatingPlaceCount - 1].name, hotelName);

						if (eatingPlaceCount  > MAX_NODE_TAGS)
						{
							cout << "too much food ";
							exit(0);
						}
						//cout << "\nA yumeeeeeeeeeeeeee";
					}
					if (!strcmp(word, "\"hospital\",") || !strcmp(word, "\"hospital\""))
					{
						char *hospName;
						//cout << "\nA medicoooooooooooo";
						storeTagInArray(hospitals, &hospitalCount, nodes[nodeCount]);
						hospName = returnName(fp);
						strcpy(hospitals[hospitalCount - 1].name, hospName);


						if (hospitalCount  > MAX_NODE_TAGS)
						{
							cout << "too much sickeness ";
							exit(0);
						}
					}
					if (!strcmp(word, "\"parking\",") || !strcmp(word, "\"parking\""))
					{
						//cout << "\nA parkkkkkkkkkkkkkkk";
						storeTagInArray(parkings, &parkingCount, nodes[nodeCount]);

						if (parkingCount  > MAX_NODE_TAGS)
						{
							cout << "too much park ";
							exit(0);
						}
					}
				}
				if (!strcmp(word, "\"bus_stop\",") || !strcmp(word, "\"bus_stop\"") || !strcmp(word, "\"bus_station\",") || !strcmp(word, "\"bus_station\""))
				{
					//cout << "\nbussssssssssssssssssssssssss";
					storeTagInArray(busStops, &busStopCount, nodes[nodeCount]);
					if (busStopCount  > MAX_NODE_TAGS)
					{
						cout << "too much bus ";
						exit(0);
					}
				}
			}


			nodeCount++;
		}
	}
	//cout << "\nno of nodes in llist" << nodeCount << "treeeeeeeees " << treeCount;
	firstNode.x = convert_lon_to_x(27.1753020);
	firstNode.y = convert_lat_to_y(78.0433751);


}

node *searchNode(long long nodeID)
{
	bool found = 0;
	for (int i = 0; i < map.nodeCount; i++)
	{
		if (nodeID == map.nodes[i].id)
		{
			found = 1;
			return &map.nodes[i];
		}
	}

	if (!found)
	{
		cout << "\nSystem ERROR !! not found : " << nodeID;
		exit(0);
		return 0;
	}

}

// this function stores the ways from nodes array
void Map::store_ways_in_array(FILE *fp)
{
	cout << "storing ways and buildings...";
	ways = new way[MAX_WAYS];
	way_blgs = new way[MAX_BUILDINGS];
	blgNames = new nodeTag[MAX_BUILDINGS];
	streetNames = new nodeTag[MAX_NODE_TAGS];

	int wayIndex = 0;
	int buildingIndex = 0;
	char word[1000], ch = 'a';

	// the fp continues from where store_nodes_in_array left off
	while (!feof(fp))
	{
		readWord(fp, word);

		if (!strcmp(word, "0"))
			break;

		if (!strcmp(word, "\"way\",")) // encountered "way",
		{
			// read "id:"
			readWord(fp, word);

			// read id value
			readWord(fp, word);

			// converting string to long long
			std::stringstream sstr(word);
			__int64 val;
			sstr >> val;


			ways[wayIndex].id = val;


			// read "nodes":
			readWord(fp, word);

			readWord(fp, word); // read '['

			ways[wayIndex].nodeCount = 0;

			// loop reading off and storing the nodes in a way
			ch = '[';
			while (ch != ']' && !feof(fp))
			{
				readWord(fp, word);
				std::stringstream sstr(word);
				__int64 val;
				sstr >> val;

				ways[wayIndex].nodeList[ways[wayIndex].nodeCount] = searchNode(val);

				ways[wayIndex].nodeCount++;

				readWhiteSpaces(fp);
				fscanf(fp, "%c", &ch); // check if its the ] following the last node value
				if (ch != ']')
					fseek(fp, -1, SEEK_CUR);
			}
			// read ,
			fscanf(fp, "%c", &ch);
			readWord(fp, word);

			if (!strcmp(word, "\"tags\":"))
			{
				// read the tags till you encounter some street element
				while (strcmp(word, "},"))
				{
					readWord(fp, word);
					if (!strcmp(word, "}") || !strcmp(word, "0"))
						break;


					if ((!strcmp(word, "\"building\":") || !strcmp(word, "\"school\",") || !strcmp(word, "\"college\",") || !strcmp(word, "\"hospital\",") || !strcmp(word, "\"hospital\"")) && buildingIndex < MAX_BUILDINGS)
					{
						char *buildingName;
						//cout << "booooooooooooooooooooo";
						way_blgs[buildingIndex] = ways[wayIndex];
						buildingIndex++;

						if (buildingIndex == MAX_BUILDINGS)
						{
							cout << "\nbuilding count greater than max building limit, try somewhere less congested";
							wayCount = wayIndex;
							blgCount = buildingIndex;
							cout << "\n\nNo of ways in this data : " << wayCount;
							return;
						}
						storeTagInArray(blgNames, &blgCount, *ways[wayIndex].nodeList[0]);
						buildingName = returnName(fp);
						strcpy(blgNames[blgCount - 1].name, buildingName);

						//if(strcmp(buildingName, "empty"))
						cout << "\n" << buildingName;

						// for this needs not be drawn as a way
						//wayIndex--;
						//cout << "\nbuildingIndex "<<buildingIndex;
					}

					if (!strcmp(word, "\"highway\":"))
					{
						char *streetName;
						storeTagInArray(streetNames, &streetCount, *ways[wayIndex].nodeList[0]);
						streetName = returnName(fp);
						strcpy(streetNames[streetCount - 1].name, streetName);
					}

					/* Take care of oneway after highway names
					if(!strcmp(word, "\"oneway\":"))
					{
					readWord(fp, word);
					if(!strcmp(word, "\"yes\",") || !strcmp(word, "\"yes\""))
					{
					cout << "\nonewayyyyyyyyyyyyyyyyyyy";
					storeTagInArray(oneways, &onewayCount, *ways[wayIndex].nodeList[0]);
					}
					}
					*/

				}
			}

			// one more way gone into ways array
			wayIndex++;
			if (wayIndex == MAX_WAYS)
			{
				cout << "\way count greater than max way limit, try somewhere less congested";
				wayCount = wayIndex;
				blgCount = buildingIndex;
				cout << "\n\nNo of ways in this data : " << wayCount;
				return;
			}

		}
	}
	wayCount = wayIndex;
	blgCount = buildingIndex;
	cout << "\n\nNo of ways in this data : " << wayCount;
}


void computeRectangleFromLineAndStore(float x1, float y1, float x2, float y2)
{
	float width = 1;
	double thetaSlope;

	float roadBase = x1 - x2;
	float roadPerp = y2 - y1;

	thetaSlope = atan(roadPerp / roadBase);

	float base = width * sin(thetaSlope);
	float perp = width * cos(thetaSlope);

	map.rects[map.rectCount].thetaSlope = thetaSlope;
	map.rects[map.rectCount].x1_right = x1 + base;
	map.rects[map.rectCount].y1_right = y1 + perp;

	map.rects[map.rectCount].x2_right = x2 + base;
	map.rects[map.rectCount].y2_right = y2 + perp;

	map.rects[map.rectCount].x1_left = x1 - base;
	map.rects[map.rectCount].y1_left = y1 - perp;

	map.rects[map.rectCount].x2_left = x2 - base;
	map.rects[map.rectCount].y2_left = y2 - perp;
}


void Map::store_rect_coords_in_array()
{
	int x, y;
	map.rects = new rectangle[MAX_NODES];

	for (int i = 0; i < map.wayCount; i++)
	{
		float prev_x = INFINITY, prev_y = INFINITY;
		map.rects[map.rectCount - 1].endRect = true;

		for (int j = 0; j < map.ways[i].nodeCount; j++)
		{
			x = map.ways[i].nodeList[j]->x;
			y = map.ways[i].nodeList[j]->y;

			if (prev_x != INFINITY && prev_y != INFINITY)
			{
				computeRectangleFromLineAndStore(prev_x, prev_y, x, y);
				map.rects[map.rectCount].endRect = false;
				map.rectCount++;
			}
			prev_x = x;
			prev_y = y;
		}
	}
}

// stores the values in float array will will help fill the batch
void fillRectArray(GLfloat rectArray[MAX_WAYS * 6][3], GLfloat texRoadArray[MAX_WAYS * 6][2], rectangle rectCurr, int *index)
{
	int j = *index;
	// fill in rectArray
	// 0
	rectArray[j][0] = rectCurr.x2_left;
	rectArray[j][1] = 0;
	rectArray[j][2] = rectCurr.y2_left;

	texRoadArray[j][0] = 0;
	texRoadArray[j][1] = 0;

	j++;

	// 1

	rectArray[j][0] = rectCurr.x2_right;
	rectArray[j][1] = 0;
	rectArray[j][2] = rectCurr.y2_right;

	texRoadArray[j][0] = 1;
	texRoadArray[j][1] = 0;


	j++;

	// 2
	rectArray[j][0] = rectCurr.x1_left;
	rectArray[j][1] = 0;
	rectArray[j][2] = rectCurr.y1_left;

	texRoadArray[j][0] = 0;
	texRoadArray[j][1] = 1;

	j++;

	// 3
	rectArray[j][0] = rectCurr.x2_right;
	rectArray[j][1] = 0;
	rectArray[j][2] = rectCurr.y2_right;

	texRoadArray[j][0] = 1;
	texRoadArray[j][1] = 0;

	j++;

	// 4
	rectArray[j][0] = rectCurr.x1_left;
	rectArray[j][1] = 0;
	rectArray[j][2] = rectCurr.y1_left;

	texRoadArray[j][0] = 0;
	texRoadArray[j][1] = 1;

	j++;

	// 5
	rectArray[j][0] = rectCurr.x1_right;
	rectArray[j][1] = 0;
	rectArray[j][2] = rectCurr.y1_right;

	texRoadArray[j][0] = 1;
	texRoadArray[j][1] = 1;

	j++;

	*index = j;
}

void fillJointArray(GLfloat jointArray[MAX_WAYS * 6][3], GLfloat texJointArray[MAX_WAYS * 6][2], rectangle rectCurr, rectangle rectPrev, int *index)
{
	int j = *index;

	// fill in rectArray
	// 0
	jointArray[j][0] = rectPrev.x2_left;
	jointArray[j][1] = 0;
	jointArray[j][2] = rectPrev.y2_left;

	texJointArray[j][0] = 0;
	texJointArray[j][1] = 0;

	j++;

	// 1	
	jointArray[j][0] = rectPrev.x2_right;
	jointArray[j][1] = 0;
	jointArray[j][2] = rectPrev.y2_right;

	texJointArray[j][0] = 1;
	texJointArray[j][1] = 0;

	j++;

	// 2
	jointArray[j][0] = rectCurr.x1_right;
	jointArray[j][1] = 0;
	jointArray[j][2] = rectCurr.y1_right;


	texJointArray[j][0] = 0;
	texJointArray[j][1] = 1;


	j++;

	// 3
	jointArray[j][0] = rectPrev.x2_left;
	jointArray[j][1] = 0;
	jointArray[j][2] = rectPrev.y2_left;

	texJointArray[j][0] = 0;
	texJointArray[j][1] = 0;

	j++;

	// 4
	jointArray[j][0] = rectPrev.x2_right;
	jointArray[j][1] = 0;
	jointArray[j][2] = rectPrev.y2_right;

	texJointArray[j][0] = 1;
	texJointArray[j][1] = 0;

	j++;

	// 5
	jointArray[j][0] = rectCurr.x1_left;
	jointArray[j][1] = 0;
	jointArray[j][2] = rectCurr.y1_left;

	texJointArray[j][0] = 1;
	texJointArray[j][1] = 1;

	j++;

	*index = j;
}

// computes the normal of the rectangle
vec3 computeNormal(node n1, node n2)
{
	vec3 wallBase = vec3(n1.x - n2.x, 0, n1.y - n2.y);

	// normal is perpendicular to wall base, therefore the cross product of wallBase with y axis
	vec3 normal = glm::cross(wallBase, vec3(0, 1, 0));

	normal = glm::normalize(normal);
	return normal;
}

void createRectangleAndStoreInFloatArray(M3DVector3f *buildingArray, M3DVector2f *texBlgArray, M3DVector3f *normals, node n1, node n2, int *index, float center_x, float center_y, int height)
{
	int j = *index;


	// calulate the normal for lighting purpose
	vec3 normal = computeNormal(n1, n2);

	float wallLength = sqrt((n1.x - n2.x) * 	(n1.x - n2.x) + (n1.y - n2.y) * (n1.y - n2.y));
	float cons = 0.14;

	// the length after which the texture will repeat
	float repeat_after_length = wallLength;

	// fill in rectArray
	// 0
	buildingArray[j][0] = n1.x;
	buildingArray[j][1] = 0;
	buildingArray[j][2] = n1.y;

	texBlgArray[j][0] = 0;
	texBlgArray[j][1] = 0;

	normals[j][0] = normal.x;
	normals[j][1] = normal.y;
	normals[j][2] = normal.z;

	j++;

	// 1	
	buildingArray[j][0] = n2.x;
	buildingArray[j][1] = 0;
	buildingArray[j][2] = n2.y;

	texBlgArray[j][0] = repeat_after_length * cons;
	texBlgArray[j][1] = 0;

	normals[j][0] = normal.x;
	normals[j][1] = normal.y;
	normals[j][2] = normal.z;

	j++;

	// 2
	buildingArray[j][0] = n1.x;
	buildingArray[j][1] = height;
	buildingArray[j][2] = n1.y;

	texBlgArray[j][0] = 0;
	texBlgArray[j][1] = height * cons;


	normals[j][0] = normal.x;
	normals[j][1] = normal.y;
	normals[j][2] = normal.z;

	j++;

	// 3
	buildingArray[j][0] = n2.x;
	buildingArray[j][1] = 0;
	buildingArray[j][2] = n2.y;

	texBlgArray[j][0] = repeat_after_length * cons;
	texBlgArray[j][1] = 0;


	normals[j][0] = normal.x;
	normals[j][1] = normal.y;
	normals[j][2] = normal.z;

	j++;

	// 4
	buildingArray[j][0] = n1.x;
	buildingArray[j][1] = height;
	buildingArray[j][2] = n1.y;

	texBlgArray[j][0] = 0;
	texBlgArray[j][1] = height * cons;


	normals[j][0] = normal.x;
	normals[j][1] = normal.y;
	normals[j][2] = normal.z;

	j++;

	// 5
	buildingArray[j][0] = n2.x;
	buildingArray[j][1] = height;
	buildingArray[j][2] = n2.y;

	texBlgArray[j][0] = repeat_after_length * cons;
	texBlgArray[j][1] = height * cons;


	normals[j][0] = normal.x;
	normals[j][1] = normal.y;
	normals[j][2] = normal.z;

	j++;


	// also the contribution of this wall to the roof
	// 1
	buildingArray[j][0] = n2.x;
	buildingArray[j][1] = height;
	buildingArray[j][2] = n2.y;

	texBlgArray[j][0] = 0;
	texBlgArray[j][1] = 0;

	// normals of roof
	normals[j][0] = 0;
	normals[j][1] = 1;
	normals[j][2] = 0;


	j++;

	// 2
	buildingArray[j][0] = n1.x;
	buildingArray[j][1] = height;
	buildingArray[j][2] = n1.y;

	texBlgArray[j][0] = 0;
	texBlgArray[j][1] = 0;

	normals[j][0] = 0;
	normals[j][1] = 1;
	normals[j][2] = 0;

	j++;

	// 3
	buildingArray[j][0] = center_x;
	buildingArray[j][1] = height;
	buildingArray[j][2] = center_y;

	texBlgArray[j][0] = 0;
	texBlgArray[j][1] = 0;

	normals[j][0] = 0;
	normals[j][1] = 1;
	normals[j][2] = 0;

	*index = j;
}

void Map::store_buildings_in_batch()
{
	int building_max_height = 9;
	M3DVector3f *normals;
	M3DVector3f *buildingArray;
	M3DVector2f *texBlgArray;

	normals = new M3DVector3f[map.blgCount * 9 * MAX_WALLS_IN_BUILDING];
	buildingArray = new M3DVector3f[map.blgCount * 9 * MAX_WALLS_IN_BUILDING];
	texBlgArray = new M3DVector2f[map.blgCount * 9 * MAX_WALLS_IN_BUILDING];


	int k = 0;
	for (int i = 0; i < map.blgCount; i++)
	{
		// height of ith building
		int height = rand() % building_max_height;

		// height cannot be less than 4
		while (height < 4)
		{
			height = rand() % building_max_height;
		}
		// draw all the walls of the building
		for (int j = 0; j < way_blgs[i].nodeCount - 1; j++)
		{
			createRectangleAndStoreInFloatArray(buildingArray, texBlgArray, normals, *way_blgs[i].nodeList[j], *way_blgs[i].nodeList[j + 1], &k, way_blgs[i].nodeList[0]->x, way_blgs[i].nodeList[0]->y, height);
			k++;
		}
	}


	building.batch.Begin(GL_TRIANGLES, map.blgCount * 9 * MAX_WALLS_IN_BUILDING, 1);
	building.batch.CopyVertexData3f(buildingArray);
	building.batch.CopyTexCoordData2f(texBlgArray, 0);
	building.batch.CopyNormalDataf(normals);
	building.batch.End();


	// deleting the arrays allocated by new
	delete[] buildingArray;
	delete[] texBlgArray;
}

// stores coords in batch of roads and joints taken from the map.rects
void Map::store_rects_in_batch()
{
	// the float arrays from which the batch is filled

	GLfloat(*rectArray)[3] = new GLfloat[MAX_WAYS * 6][3];
	GLfloat(*texRoadArray)[2] = new GLfloat[MAX_WAYS * 6][2];

	GLfloat(*jointArray)[3] = new GLfloat[MAX_WAYS * 6][3];
	GLfloat(*texJointArray)[2] = new GLfloat[MAX_WAYS * 6][2];


	int j = 0;
	int k = 0;

	rectangle rectCurr, rectPrev;

	for (int i = 0; i < map.rectCount; i++)
	{
		rectCurr = map.rects[i];

		fillRectArray(rectArray, texRoadArray, rectCurr, &j);

		// fill in jointArray if ending is false then only join the ways		
		if (map.rects[i - 1].endRect == false)
		{
			rectPrev = map.rects[i - 1];
			fillJointArray(jointArray, texJointArray, rectCurr, rectPrev, &k);
		}
	}

	// fill in the roads model's batch
	roads.batch.Begin(GL_TRIANGLES, map.rectCount * 6, 1);
	roads.batch.CopyVertexData3f(rectArray);
	roads.batch.CopyTexCoordData2f(texRoadArray, 0);
	roads.batch.End();


	// fill in the joints model's batch
	joints.batch.Begin(GL_TRIANGLES, map.rectCount * 6, 1);
	joints.batch.CopyVertexData3f(jointArray);
	joints.batch.CopyTexCoordData2f(texJointArray, 0);
	joints.batch.End();

}

