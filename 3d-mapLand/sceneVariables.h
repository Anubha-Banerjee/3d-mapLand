#include <GLTools.h>

struct uniform
{
	GLuint ambientcol ; 
	GLuint diffusecol ; 
	GLuint specularcol ; 
	GLuint shininesscol ; 
	GLint shader;
};

extern GLint shader;
extern uniform colorUniform;
extern GLuint lightposn; 
extern GLuint lightcolor;
