#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <GLTools.h>

typedef glm::mat3 mat3 ;
typedef glm::mat4 mat4 ; 
typedef glm::vec3 vec3 ; 
typedef glm::vec4 vec4 ; 

class Model {
public:
	GLBatch batch;
	float size ;
	GLfloat ambient[4]; 
	GLfloat diffuse[4];
	GLfloat specular[4]; 
	GLfloat shininess;
    mat4 transform ;
	mat4 scale;
	mat4 translate;
	mat4 rotate;
	vec4 position, positionPrev;
	GLuint texture;

	// fills the batch and initialize the colors, transforms
	void initializeModelFromObjectFile(char *objectFile, char * textureFile, float amb[4], float diff[4], float spec[4], float shine, mat4 transl, mat4 scale);
	void initializeModelFromFloatArray(char * textureFile, float amb[4], float diff[4], float spec[4], float shine, mat4 transf);
	bool LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode);
} ;

extern Model land, roads, joints,t_light, hotel_blg, building, roundabout, eatingPlaceSign, hospitalSign, parkingSign, busSign, onewaySign, placard, blgSign, pole, tree;
#endif