#include "camera.h"
#include <iostream>



void Camera::rotateCamera(vec4 circleCenter, mat3 rotn, vec3 carCentre)
{
	// traslating origin to change circleCenter as car rotates about the circle of rotation, so camera also rotates around same centre 	

	vec3 eyeNew;
	eyeNew = myCamera.eye - vec3(circleCenter);
	eyeNew = rotn * eyeNew;	
	myCamera.eye = eyeNew + vec3(circleCenter);


	vec3 goalNew;
	goalNew = myCamera.goal - vec3(circleCenter);
	goalNew = rotn * goalNew;	
	myCamera.goal = goalNew + vec3(circleCenter);

	vec3 carNew;
	carNew = carCentre - vec3(circleCenter);
	carNew = rotn * carNew;	
	carCentre = carNew + vec3(circleCenter);
	
}