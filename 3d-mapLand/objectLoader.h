#include <GLTools.h>

struct component
{
	float x;
	float y;
	float z;
};

extern component *v,*vn,*vt;
extern M3DVector3f *Ver;
extern M3DVector3f *Normals;
extern M3DVector2f *vTexCoords; 	

int loadMesh(FILE *fp);