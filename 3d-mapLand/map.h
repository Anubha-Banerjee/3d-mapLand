#include <GL/glew.h>
#define MAX_NODES 100000
#define MAX_WAYS  100000
#define MAX_NODES_IN_WAY 2000
#define SCALE 9000
#define M_PI 3.14159265359
#define INFINITY 1000000
#define MAX_WALLS_IN_BUILDING 1000
#define MAX_BUILDINGS 2000
#define MAX_NODE_TAGS 2000

// the dimension of map portion to be loaded at a time
static const double length = 0.02866;
static const double width = 0.0209;


struct node
{
	long long id;
	int x, y;
};

struct nodeTag
{
	int x, y;
	char name[1000];
};

// can easily store the mid points of rect too later..
// contains all the 4 coordinates of rectangle making up the road
struct rectangle
{
	float x1_left, y1_left;
	float x1_right, y1_right;

	float x2_left, y2_left;
	float x2_right, y2_right;

	bool endRect;
	double thetaSlope;
};

struct way
{
	long long  id;
	int nodeCount;

	// for all the nodes in the way
	node *nodeList[MAX_NODES_IN_WAY];
};

class Map
{
public:
	// the top left latitutude and longitude of the bounding box, change
	double map_latitude;
	double map_longitude;

	// TODO : make it dynamic allocation
	// array of nodes present in map
	node *nodes, firstNode;
	way *ways, *way_blgs;
	nodeTag *trafficLights, *roundabouts, *eatingPlaces, *hospitals, *parkings, *busStops, *oneways, *blgNames, *streetNames, *trees;
	char file[100];

	rectangle *rects;

	// actual nodes and ways in array
	int nodeCount;
	int wayCount;
	int rectCount;
	int trafficLightCount, roundaboutCount, eatingPlaceCount, hospitalCount, parkingCount, busStopCount, onewayCount, streetCount, treeCount;
	int blgCount;
	float skyColor[3];
	bool showStreetNames;


	// input the coordinates the user wants to drive in  
	bool inputCoordinates();
	void store_nodes_in_array(FILE *fp);
	void store_ways_in_array(FILE *fp);
	void store_rect_coords_in_array();

	// we require to store coordinates in batch.take batch to be filled as input param 
	void store_rects_in_batch();
	void store_buildings_in_batch();


	// the shift to bring the map to center of the screen
	void calculateShift(double x, double y);

	// the values of x and y to be shifted to bring the map on screen
	double shiftX, shiftY;

	Map()
	{
		showStreetNames = true;
		strcpy(file, "");
		//trafficLightCount, roundaboutCount, eatingPlaceCount, hospitalCount, parkingCount, busStopCount;
	}
};

extern Map map;