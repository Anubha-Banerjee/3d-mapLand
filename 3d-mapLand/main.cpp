#include <iostream>
#include <string>
#include <GL/glew.h>
#include <GL/glut.h>
#include "connection.h"
#include "camera.h"
#include "transform.h"
#include "model.h"
#include "map.h"
#include "car.h"
#include "sceneVariables.h"
#include "sounds.h"
#include <time.h>


#define WIDTH 1000
#define HEIGHT 1000

Model land, roads, joints, t_light, building, roundabout, eatingPlaceSign, hospitalSign, parkingSign, busSign, onewaySign, placard, blgSign, pole, tree;
Camera myCamera;
Map map;
Car myCar;

GLuint test;
GLint shader;
GLuint lightposn;
GLuint lightcolor;
uniform colorUniform;
// a variable to check if collision is occuring this frame
int collidingThisFrame = false;
// turn on/off Collision Detection
bool collision_detection = true, first_frame = true;
void display();
sounds engine("sounds\\standard.mp3"), background("sounds\\background.mp3"), horn("sounds\\honk.wav"), crash("sounds\\crash.wav"), reverse_gear("sounds\\reverse.wav");

void reshape(int width, int height){

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60, width / (float)height, 0.1, 1000);
	glViewport(0, 0, width, height);
}



void initializeObject(mat4 scale, Model *mod, char textureFile[50], char objFile[50])
{
	float amb[4] = { 1, 1, 1, 1 };
	float diff[4] = { 0.5, 0.5, 0.5, 1 };
	float spec[4] = { 0, 0, 0, 1 };
	float shine = 200;

	mat4 transl(1);
	mod->initializeModelFromObjectFile(objFile, textureFile, amb, diff, spec, shine, transl, scale);
}

void initializeLand()
{
	int w = 2000;

	float amb[4] = { 1, 1, 1, 1 };
	float diff[4] = { 0.5, 0.5, 0.5, 1 };
	float spec[4] = { 0, 0, 0, 1 };
	float shine = 200;

	// fill in the light properties of land
	for (int i = 0; i < 4; i++)
	{
		land.ambient[i] = amb[i];
		land.diffuse[i] = diff[i];
		land.specular[i] = spec[i];
	}

	// calculate the bottom left coordinates of the map in x and y by mercerator formula
	float lonRad_bottomLeft = m3dDegToRad(map.map_longitude);
	float latRad_bottomLeft = m3dDegToRad(map.map_latitude);
	double y_bottomLeft = (w / (2 * M_PI) * log(tan(M_PI / 4 + latRad_bottomLeft / 2)) * SCALE);
	y_bottomLeft = y_bottomLeft + map.shiftY;

	double x_bottomLeft = ((w / (2 * M_PI)) * (lonRad_bottomLeft)* SCALE);
	x_bottomLeft = -(x_bottomLeft + map.shiftX);


	// calculate the top right coordinates of the map in x and y by mercerator formula
	float lonRad_topRight = m3dDegToRad(map.map_longitude + length);
	float latRad_topRight = m3dDegToRad(map.map_latitude + width);
	double y_topRight = (w / (2 * M_PI) * log(tan(M_PI / 4 + latRad_topRight / 2)) * SCALE);
	y_topRight = y_topRight + map.shiftY;

	double x_topRight = ((w / (2 * M_PI)) * (lonRad_topRight)* SCALE);
	x_topRight = -(x_topRight + map.shiftX);


	GLfloat landVertices[6][3] = {
		x_bottomLeft, 0, y_bottomLeft,
		x_bottomLeft, 0, y_topRight,
		x_topRight, 0, y_topRight,

		x_topRight, 0, y_topRight,
		x_bottomLeft, 0, y_bottomLeft,
		x_topRight, 0, y_bottomLeft
	};

	GLfloat landNormals[6][3] = {
		0, 1, 0,
		0, 1, 0,
		0, 1, 0,

		0, 1, 0,
		0, 1, 0,
		0, 1, 0,
	};

	GLfloat landTexCoords[6][2] = {
		0, 0,
		0, 1,
		1, 1,

		1, 1,
		0, 0,
		1, 0
	};

	land.batch.Begin(GL_TRIANGLES, 6, 1);
	land.batch.CopyVertexData3f(landVertices);
	land.batch.CopyNormalDataf(landNormals);
	land.batch.CopyTexCoordData2f(landTexCoords, 0);
	land.batch.End();

	glGenTextures(1, &land.texture);
	glBindTexture(GL_TEXTURE_2D, land.texture);
	land.LoadTGATexture("brown.tga", GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
}

int hour(double lat)
{
	const int time_factor = 15;
	const int time_zone = 5;
	time_t current = time(NULL);
	int hr = (localtime(&current))->tm_hour - time_zone + lat / time_factor;
	hr = hr<0 ? 24 + hr : hr;
	return hr / 4;
}

// this function calculates the skyColor based on current time there.
void calcShade()
{

	const float sky_color[6][3] = { 0.941176, 0.972549, 1,		// azure 0 - 4
		0.878431, 1, 1,				// lightcyan between 4-8hrs
		0, 0.74902, 1,				// deep sky blue 8-12hrs
		1, 0.843137, 0,				// gold  between 12-16hrs
		0.27451, 0.509804, 0.705882,// steelblue 16-20 hrs
		0, 0, 0,					// black 20-24hrs								
	};



	int hr = hour(map.map_longitude);
	map.skyColor[0] = 0;
	map.skyColor[1] = 0.74902;
	map.skyColor[2] = 1;
}


void setup()
{
	glEnable(GL_NORMALIZE);
	glDisable(GL_CULL_FACE);
	glLineWidth(3);


	// create the land object
	mat4 scale(1);

	// create traffic light object
	scale = Transform::scale(0.07, 0.07, 0.07);
	initializeObject(scale, &t_light, "rose.tga", "trafficLight.obj");

	// create roundabout objects
	initializeObject(scale, &roundabout, "roundabout.tga", "roundabout.obj");

	// create tree objects
	scale = Transform::scale(0.1, 0.1, 0.1);
	initializeObject(scale, &tree, "darkGray.tga", "light.obj");

	// create eating place sign object
	scale = Transform::scale(0.1, 0.1, 0.1);
	initializeObject(scale, &eatingPlaceSign, "food.tga", "tallSign.obj");

	// create hospital sign object
	initializeObject(scale, &hospitalSign, "hosp.tga", "tallSign.obj");

	// create parking sign object
	initializeObject(scale, &parkingSign, "park.tga", "info_sign.obj");

	// create bus sign object
	initializeObject(scale, &busSign, "bus.tga", "info_sign.obj");

	// create oneway sign object
	initializeObject(scale, &onewaySign, "oneway.tga", "info_sign.obj");

	scale = Transform::scale(0.3, 1, 0.2);
	// create pole object
	initializeObject(scale, &pole, "rose.tga", "pole.obj");

	// create placard object
	scale = Transform::scale(1, 1, 1);
	initializeObject(scale, &placard, "wall.tga", "placard.obj");


	// Initialize shaders
	shader = gltLoadShaderPairWithAttributes("ver.vp", "frag.fp", 3, GLT_ATTRIBUTE_VERTEX, "vertex", GLT_ATTRIBUTE_NORMAL, "normal", GLT_ATTRIBUTE_TEXTURE0, "texture");

	lightposn = glGetUniformLocation(shader, "lightposn");
	lightcolor = glGetUniformLocation(shader, "lightcolor");
	colorUniform.ambientcol = glGetUniformLocation(shader, "ambient");
	colorUniform.diffusecol = glGetUniformLocation(shader, "diffuse");
	colorUniform.specularcol = glGetUniformLocation(shader, "specular");
	colorUniform.shininesscol = glGetUniformLocation(shader, "shininess");



	glClearColor(0.603922, 0.803922, 0.196078, 1);
	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(-WIDTH / 2, WIDTH / 2, -HEIGHT / 2, HEIGHT / 2);

	bool connect = map.inputCoordinates();
	FILE *fp = fopen(map.file, "r+");

	calcShade();

	// create the query to retrive data from server 
	std::string query = createQuery(map.map_latitude, map.map_longitude);


	// connect and retrive the response data which will be saved to the disk on a file
	if (connect)
		connect_and_save(query, fp);

	map.calculateShift(map.map_latitude, map.map_longitude);

	initializeLand();

	// store the nodes and ways in array data structures
	map.store_nodes_in_array(fp);
	map.store_ways_in_array(fp);

	// store the rectangle coordintes in rectangle data structure
	map.store_rect_coords_in_array();

	map.store_rects_in_batch();

	map.store_buildings_in_batch();

	float amb[4] = { 1, 1, 1, 1 };
	float diff[4] = { 0.5, 0.5, 0.5, 1 };
	float spec[4] = { 0, 0, 0, 1 };
	float shine = 200;
	mat4 transf(1.0);

	// store the all in the batch which we will draw, from rectangle data structure	
	roads.initializeModelFromFloatArray("rd2.tga", amb, diff, spec, shine, transf);
	joints.initializeModelFromFloatArray("r2.tga", amb, diff, spec, shine, transf);

	spec[0] = 1;
	spec[1] = 1;
	spec[2] = 1;
	shine = 120;
	building.initializeModelFromFloatArray("blg.tga", amb, diff, spec, shine, transf);
	//	myCar.init_x = map.firstNode.x;
	//	myCar.init_y = map.firstNode.y;
	myCar.initializeCar();


	//TOP VIEW
	/*
	// intialise camera
	myCamera.eye = vec3(0, 300,10);
	myCamera.up = vec3(0,1,0);
	myCamera.goal = vec3(0,0,0);
	*/

	// intialise camera		
	myCamera.eye = vec3(0, 0.7, 5);
	myCamera.up = vec3(0, 1, 0);
	myCamera.goal = vec3(0, 0, 0);

	background.engine->setSoundVolume(0.3);
	background.play(true);

}

float linePointPosition2D(float x1, float y1, float x2, float y2, float x3, float y3)
{
	return ((x2 - x1) * (y3 - y1)) - ((y2 - y1) * (x3 - x1));
}


void detectCollision()
{
	int intersectionCount = 0;
	float x1, y1, x2, y2, minX;

	float x = myCar.carBody[0].position.x;
	float y = myCar.carBody[0].position.z;

	float answer;


	collidingThisFrame = false;
	for (int i = 0; i < map.blgCount; i++)
	{
		intersectionCount = 0;
		for (int j = 0; j < map.way_blgs[i].nodeCount - 1; j++)
		{
			x1 = map.way_blgs[i].nodeList[j]->x;
			y1 = map.way_blgs[i].nodeList[j]->y;

			x2 = map.way_blgs[i].nodeList[j + 1]->x;
			y2 = map.way_blgs[i].nodeList[j + 1]->y;


			bool isLeft = false;
			answer = linePointPosition2D(x1, y1, x2, y2, x, y);
			float signOfLeft = linePointPosition2D(x1, y1, x2, y2, x1 - 1, y1);

			if ((signOfLeft < 0 && answer < 0) || (signOfLeft > 0 && answer > 0))
			{
				isLeft = true;
			}

			if (((y >= y1 && y < y2) || (y < y1 && y >= y2)) && isLeft)
			{
				intersectionCount++;
			}
		}

		// odd number of intersections
		if (intersectionCount % 2 != 0)
		{
			collidingThisFrame = true;
			// if the collision is for the first time with a building, no need to respond again and again
			if (myCar.colliding == false)
			{
				cout << "\n\n\nCOLLISION !!! ";
				crash.play(false);
				myCar.respondToCollision();
			}
		}
	}
}

void loopStep()
{
	if (collision_detection)
		detectCollision();


	if (first_frame == true)
	{
		// If  initially collision is true
		while (collidingThisFrame)
		{
			cout << myCar.carBody[0].position.x << "\n";
			myCar.accelerating = true;
			myCar.de_accelerating = false;
			myCar.updatePosition();
			detectCollision();
		}

		first_frame = false;
		myCar.accelerating = false;
		myCar.de_accelerating = true;
	}

	if (myCar.colliding == true)
	{
		if (myCar.linearVelocity <= 0 && collidingThisFrame == false)
		{
			myCar.colliding = false;
		}
	}

	myCar.updatePosition();
	display();
}


unsigned char prev_view = '6';
void keyPressed(unsigned char key, int x, int y)
{

	if (key == 27)
	{
		exit(0);
	}

	if (key == '1')
	{
		myCar.currentGear = 1;
	}

	if (key == '2')
	{
		myCar.currentGear = 2;
	}


	if (key == 'c')
	{

		if (collision_detection)
			collision_detection = false;
		else
			collision_detection = true;
	}

	if (key == '4')
	{
		if (map.showStreetNames)
			map.showStreetNames = false;
		else
			map.showStreetNames = true;
	}

	if (key == '5')
	{
		// so that goal is restored to goal.y-20
		if (prev_view == '3')
		{
			myCamera.goal = vec3(myCamera.goal.x, myCamera.goal.y - 20, myCamera.goal.z);
		}
		myCamera.eye = vec3(myCamera.eye.x, 300, myCamera.eye.z);
		prev_view = '5';

	}

	if (key == '6')
	{
		// so that goal is restored to goal.y-20
		if (prev_view == '3')
		{
			myCamera.goal = vec3(myCamera.goal.x, myCamera.goal.y - 20, myCamera.goal.z);
		}

		myCamera.eye = vec3(myCamera.eye.x, 0.7, myCamera.eye.z);
		prev_view = '6';
	}

	if (key == '3')
	{
		if (prev_view != '3')
		{
			myCamera.eye = vec3(myCamera.eye.x, 0.7 + 20, myCamera.eye.z);
			myCamera.goal = vec3(myCamera.goal.x, myCamera.goal.y + 20, myCamera.goal.z);
			prev_view = '3';
		}
	}

	if (key == 'h')
	{
		horn.play(false);
	}

	if (key == 'w')
	{
		myCamera.eye = myCamera.eye + vec3(0, 4, 0);
	}

	if (key == 's')
	{
		myCamera.eye = myCamera.eye - vec3(0, 4, 0);
	}
	if (key == 'f') {
		glutFullScreen();
	}


}

void specialUp(int key, int x, int y) {
	switch (key) {
	case 100: //left
		myCar.turningLeft = false;
		break;
	case 101: //up
		myCar.accelerating = false;
		myCar.de_accelerating = true;
		engine.pause_play();
		break;
	case 102: //right
		myCar.turningRight = false;
		break;
	case 103: //down		
		myCar.accelerating = false;
		myCar.de_accelerating = true;
		break;
	}
}

void specialDown(int key, int x, int y) {

	switch (key) {

	case 100: //left
		myCar.turningRight = false;
		myCar.turningLeft = true;
		break;

	case 101: //up
		if (myCar.colliding == false)
		{
			engine.play(true);
			myCar.accelerating = true;
			myCar.de_accelerating = false;
		}
		if (myCar.linearVelocity == 0)
		{
			myCar.reverseGear = false;
			reverse_gear.pause_play();
		}
		if (myCar.reverseGear == true)
		{
			myCar.accelerating = false;
			myCar.de_accelerating = true;
		}
		break;
	case 102: //right
		myCar.turningRight = true;
		myCar.turningLeft = false;
		break;
	case 103: //down

		if (myCar.linearVelocity == 0)
		{
			myCar.reverseGear = true;
			reverse_gear.pause_play();
			reverse_gear.play(true);
		}
		if (myCar.reverseGear == false)
		{
			myCar.accelerating = false;
			myCar.de_accelerating = true;
		}
		else
		{

			myCar.accelerating = true;
			myCar.de_accelerating = false;
		}
		break;
	}
}

void deAllocateMemory()
{
	delete[] map.nodes;
	delete[] map.ways;
	delete[] map.way_blgs;
	delete[] map.trafficLights;
	delete[] map.rects;
	delete[] map.busStops;
	delete[] map.eatingPlaces;
	delete[] map.hospitals;
	delete[] map.oneways;
	delete[] map.parkings;
	delete[] map.roundabouts;
	delete[] map.blgNames;
	delete[] map.streetNames;
	delete[] map.trees;
}

void retriveCoordinatesFromCommandLine(char argv[100])
{
	char latAngLong[100];
	char lat[100], lon[100];
	strcpy(latAngLong, argv);

	//cout << "\n lat&lon" << latAngLong;

	int i = 0;


	// get the latitude
	while (latAngLong[i] != ' ')
	{
		lat[i] = latAngLong[i];
		i++;
	}
	lat[i] = NULL;
	cout << "lat is : " << lat;
	map.map_latitude = atof(lat);
	int j = 0;
	// get the longitude
	while (latAngLong[i] != 'x')
	{
		lon[j] = latAngLong[i];
		i++;
		j++;
	}

	lon[j] = NULL;
	cout << "\nlon is : " << lon;
	map.map_longitude = atof(lon);
}

void main(int argc, char **argv)
{

	//retriveCoordinatesFromCommandLine(argv[1]);	

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowPosition(0, 0);

	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("3d map land");
	

	glutDisplayFunc(loopStep);
	glutReshapeFunc(reshape);


	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialDown);
	glutSpecialUpFunc(specialUp);


	glewInit();
	setup();
	glutMainLoop();
	deAllocateMemory();

}