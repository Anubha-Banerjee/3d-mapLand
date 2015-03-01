#include <GL/glew.h>
#include <GL/glut.h>
#include "model.h"
#include "car.h"
#include "map.h"
#include "sceneVariables.h"
#include "transform.h"
#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

using namespace std;


void transformvec(const GLfloat input[4], GLfloat output[4])
{
	GLfloat modelview[16]; // in column major order
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

	for (int i = 0; i < 4; i++) {
		output[i] = 0;
		for (int j = 0; j < 4; j++) {
			output[i] += modelview[4 * j + i] * input[j];
		}
	}
}

void setUniform(mat4 transform, GLfloat amb[4], GLfloat diff[4], GLfloat spec[4], float shine, GLuint texture)
{
	glLoadMatrixf(&transform[0][0]);
	glUniform4fv(colorUniform.ambientcol, 1, amb);
	glUniform4fv(colorUniform.diffusecol, 1, diff);
	glUniform4fv(colorUniform.specularcol, 1, spec);
	glUniform1fv(colorUniform.shininesscol, 1, &shine);
	glBindTexture(GL_TEXTURE_2D, texture);
}
void drawName(Model *mod, int count, nodeTag *arr, mat4 mv, char name[1000])
{
	char *p;
	int stringLength;

	GLfloat ambient[4] = { 0, 0, 0, 0 };
	stringLength = strlen(name);

	// draw the font
	mat4 scale = Transform::scale(0.04, 0.04, 0.04);
	mat4 translate = Transform::translate(-stringLength / 2, 8.5, 0.5);
	setUniform(mv *  mod->translate * translate * mod->scale * scale, mod->ambient, mod->diffuse, mod->specular, mod->shininess, mod->texture);

	for (p = name; *p; p++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);

	float new_card_Length = stringLength + 8, card_length = 100;
	float scaleX = new_card_Length / card_length; //stringLength / orignalLength_placard;

	// draw the placard
	scale = Transform::scale(scaleX, 0.1, 0.1);
	translate = Transform::translate(0, 6, 0);
	setUniform(mv *  mod->translate * translate * placard.scale * scale, placard.ambient, placard.diffuse, placard.specular, placard.shininess, placard.texture);
	placard.batch.Draw();

	// draw the font rotated
	scale = Transform::scale(0.05, 0.05, 0.05);
	translate = Transform::translate(stringLength / 2, 8.5, -0.5);
	mat3 rotate = Transform::rotate(180, vec3(0, 1, 0));

	mat4 rotate4 = mat4(rotate);
	setUniform(mv *  mod->translate * translate * rotate4 * mod->scale * scale, mod->ambient, mod->diffuse, mod->specular, mod->shininess, mod->texture);

	for (p = name; *p; p++)
		glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *p);

}

void drawWayNames(Model *mod, int count, nodeTag *arr, mat4 mv, int displacement)
{
	char mystring[2000];
	for (int i = 0; i < count; i++)
	{
		strcpy(mystring, arr[i].name);

		// if name is to be printed then scale the info_sign
		if (strcmp(mystring, "empty"))
		{
			mod->scale = mat4(
				0.2, 0, 0, 0,
				0, 0.3, 0, 0,
				0, 0, 0.2, 0,
				0, 0, 0, 1
				);
		}
		mod->translate = Transform::translate(arr[i].x + displacement, 0, arr[i].y + displacement);
		setUniform(mv *  mod->translate * mod->scale, mod->ambient, mod->diffuse, mod->specular, mod->shininess, mod->texture);

		if (strcmp(mystring, "empty"))
		{
			mod->batch.Draw();
			drawName(mod, count, arr, mv, mystring);
		}
	}
}

void drawStreetElements(Model *mod, int count, nodeTag *arr, mat4 mv)
{
	char mystring[2000];
	for (int i = 0; i < count; i++)
	{
		strcpy(mystring, arr[i].name);

		// if name is to be printed then scale the info_sign
		if (strcmp(mystring, "empty"))
		{
			mod->scale = mat4(
				0.2, 0, 0, 0,
				0, 0.2, 0, 0,
				0, 0, 0.2, 0,
				0, 0, 0, 1
				);
		}
		mod->translate = Transform::translate(arr[i].x, 0, arr[i].y);
		setUniform(mv *  mod->translate * mod->scale, mod->ambient, mod->diffuse, mod->specular, mod->shininess, mod->texture);
		mod->batch.Draw();

		if (strcmp(mystring, "empty"))
		{
			drawName(mod, count, arr, mv, mystring);
		}
	}
}

void display()
{
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	glClearColor(map.skyColor[0], map.skyColor[1], map.skyColor[2], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	mat4 mv, transform(1.0);
	mv = glm::lookAt(myCamera.eye, myCamera.goal, myCamera.up);

	glLoadMatrixf(&mv[0][0]);

	glUseProgram(shader);

	const GLfloat light_positional[] = { 0, 200, -250, 1 };
	const GLfloat color[] = { 0.6, 0.3, 0, 1 };

	GLfloat light[4];
	transformvec(light_positional, light);

	// setting light position and color
	glUniform4fv(lightposn, 1, light);
	glUniform4fv(lightcolor, 1, color);

	// render the land
	transform = mv * land.transform;

	setUniform(transform, land.ambient, land.diffuse, land.specular, land.shininess, land.texture);
	land.batch.Draw();

	glDisable(GL_DEPTH_TEST);

	setUniform(transform, joints.ambient, joints.diffuse, joints.specular, joints.shininess, joints.texture);
	joints.batch.Draw();

	setUniform(transform, roads.ambient, roads.diffuse, roads.specular, roads.shininess, roads.texture);

	roads.batch.Draw();

	glEnable(GL_DEPTH_TEST);
	setUniform(transform, building.ambient, building.diffuse, building.specular, building.shininess, building.texture);
	building.batch.Draw();

	glColor3f(0, 0, 0);

	// draw the street elements - traffic lights 
	drawStreetElements(&t_light, map.trafficLightCount, map.trafficLights, mv);

	// draw the street elements - roundabouts
	drawStreetElements(&roundabout, map.roundaboutCount, map.roundabouts, mv);

	// draw the street elements - eatingPlace signs
	drawStreetElements(&eatingPlaceSign, map.eatingPlaceCount, map.eatingPlaces, mv);

	// draw the street elements - hospital signs
	drawStreetElements(&hospitalSign, map.hospitalCount, map.hospitals, mv);

	// draw the street elements - parking signs
	drawStreetElements(&parkingSign, map.parkingCount, map.parkings, mv);

	// draw the street elements - bus signs
	drawStreetElements(&busSign, map.busStopCount, map.busStops, mv);

	// draw the street elements - oneway signs
	drawStreetElements(&onewaySign, map.onewayCount, map.oneways, mv);

	// draw the building names stored in map.blgNames
	drawWayNames(&pole, map.blgCount, map.blgNames, mv, 0);

	// draw the trees stored in map.trees
	drawStreetElements(&tree, map.treeCount, map.trees, mv);

	if (map.showStreetNames)
	{
		// draw the street names stored in map.streetNames TODO : do something about extremely slow fps as lots of street names to render.
		drawWayNames(&pole, map.streetCount, map.streetNames, mv, 2);
	}

	// render the car body
	for (int i = 0; i < 2; i++)
	{
		transform = mv * myCar.carBody[i].transform;
		setUniform(transform, myCar.carBody[i].ambient, myCar.carBody[i].diffuse, myCar.carBody[i].specular, myCar.carBody[i].shininess, myCar.carBody[i].texture);
		myCar.carBody[i].batch.Draw();
	}

	// render the frontwheels
	for (int i = 0; i < 2; i++)
	{
		transform = mv * myCar.frontWheel[i].transform;
		setUniform(transform, myCar.frontWheel[i].ambient, myCar.frontWheel[i].diffuse, myCar.frontWheel[i].specular, myCar.frontWheel[i].shininess, myCar.frontWheel[i].texture);
		myCar.frontWheel[i].batch.Draw();
	}

	// render the rearwheels
	for (int i = 0; i < 2; i++)
	{
		transform = mv * myCar.rearWheel[i].transform;
		setUniform(transform, myCar.rearWheel[i].ambient, myCar.rearWheel[i].diffuse, myCar.rearWheel[i].specular, myCar.rearWheel[i].shininess, myCar.rearWheel[i].texture);
		myCar.rearWheel[i].batch.Draw();
	}

	glutSwapBuffers();
	glutPostRedisplay();
}
