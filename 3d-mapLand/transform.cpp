// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "transform.h"
#define PI 3.141
#include <GL/glut.h>

// Helper rotation function.  
mat3 Transform::rotate(const float degrees, const vec3& axis) {	

  mat3 rotation;
  // stores the value of degrees in radians
  double theta = -degrees * PI / 180;

  double cosTheta = cos(theta); 
  double sinTheta = sin(theta);
   glm::normalize(axis);

  // an identity matrix
  mat3 identity = mat3(
   1, 0, 0,
   0, 1, 0,
   0, 0, 1 
  );

  // the vector k is represented as a matrix see wiki rodrigue formula
  mat3 axisMatrix = mat3(
	0, -1 * axis.z, axis.y,
	axis.z, 0, -axis.x,
	-axis.y, axis.x, 0
	);

    mat3 aaT = mat3(
	  axis.x*axis.x, axis.x* axis.y, axis.x * axis.z,
	  axis.x*axis.y, axis.y*axis.y, axis.y*axis.z,
	  axis.x*axis.z, axis.y*axis.z, axis.z * axis.z
	  ); 
 
  mat3 axisMatrixTranspose =glm::transpose(axisMatrix);

  rotation = identity * cosTheta +  axisMatrix * sinTheta + aaT * (1 - cosTheta);

  return rotation;
}

void Transform::left(float degrees, vec3& eye, vec3& goal, vec3& up, vec3& carCentre) {

	vec3 goalNew = goal - carCentre;
	goalNew = rotate(degrees, up) * goalNew;
	goal = goalNew + carCentre;

		
	vec3 eyeNew = eye - carCentre;
	eyeNew = rotate(degrees, up) * eyeNew;
	eye = eyeNew + carCentre;
}

void Transform::up(float degrees, vec3& eye, vec3& up) {

	vec3 eyeNormal = glm::normalize(eye);
	vec3 upNormal =glm :: normalize(up);

	vec3 axis = glm::cross(upNormal, eyeNormal);
	axis = glm::normalize(axis);

	mat3 rotation = rotate(degrees, axis);
	eye  = rotation * eye; 
	up = rotation * up;	
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
	mat4 ret = mat4(
		sx, 0, 0, 0,
		0, sy, 0, 0,
		0, 0, sz, 0,
		0, 0, 0, 1		
	);
	return ret;
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
   mat4 ret(1,0,0,0,
	        0,1,0,0,
			0,0,1,0,
			tx,ty,tz,1);

    return ret;
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
