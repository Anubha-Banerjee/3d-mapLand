#include "model.h" 
#include "transform.h"
#include "objectLoader.h"
#include <iostream>

using namespace std;

bool Model::LoadTGATexture(const char *szFileName, GLenum minFilter, GLenum magFilter, GLenum wrapMode)
{
	GLbyte *pBits;
	int nWidth, nHeight, nComponents;
	GLenum eFormat;
	
	// Read the texture bits
	pBits = gltReadTGABits(szFileName, &nWidth, &nHeight, &nComponents, &eFormat);
	if(pBits == NULL) 
		return false;
	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, nComponents, nWidth, nHeight, 0,
				 eFormat, GL_UNSIGNED_BYTE, pBits);
	
    free(pBits);
    
    if(minFilter == GL_LINEAR_MIPMAP_LINEAR || 
       minFilter == GL_LINEAR_MIPMAP_NEAREST ||
       minFilter == GL_NEAREST_MIPMAP_LINEAR ||
       minFilter == GL_NEAREST_MIPMAP_NEAREST)
       glGenerateMipmap(GL_TEXTURE_2D);    
	return true;
}


void freePointers()
{
	free(v);
	free(vn);
	free(vt);
	free(Ver);
	free(Normals);
	free(vTexCoords);
}

void fillBuffer(char fname[40],GLBatch *batch)
{
	FILE *fp;
	fp=fopen(fname,"r+");
	int total_ver = loadMesh(fp);	
	fclose(fp);

	batch->Begin(GL_TRIANGLES,total_ver,1);
	batch->CopyVertexData3f(Ver);
	batch->CopyTexCoordData2f(vTexCoords,0);
	batch->CopyNormalDataf(Normals);	
	batch->End();
	freePointers();
}

void Model::initializeModelFromObjectFile(char *objectFile, char * textureFile, float amb[4], float diff[4], float spec[4], float shine, mat4 transl, mat4 sc)
{
	// fill in the batch of the model with v, vn and vt
	fillBuffer(objectFile, &batch);
	
	// fill in the light properties of models
	for(int i = 0; i < 4; i++)
	{
		ambient[i] = amb[i];
		diffuse[i] = diff[i];
		specular[i] = spec[i];
	}
	// models shininess
	shininess = shine;
		
	// fill models texture field
	glGenTextures(1,&texture);	
	glBindTexture(GL_TEXTURE_2D,texture);	
	LoadTGATexture(textureFile, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
		
	// TODO : change this to transform instead of translate...
	translate = transl;
	scale = sc;
}

void Model::initializeModelFromFloatArray(char * textureFile, float amb[4], float diff[4], float spec[4], float shine, mat4 transf)
{
	// fill in the light properties of models
	for(int i = 0; i < 4; i++)
	{
		ambient[i] = amb[i];
		diffuse[i] = diff[i];
		specular[i] = spec[i];
	}
	// models shininess
	shininess = shine;
		
	// fill models texture field
	glGenTextures(1,&texture);	
	glBindTexture(GL_TEXTURE_2D,texture);	
	LoadTGATexture(textureFile, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT);
		
	// TODO : change this to transform instead of translate...
	translate = transf;
}
