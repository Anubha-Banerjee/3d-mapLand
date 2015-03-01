#include <iostream>
#include "car.h"
#include "transform.h"
#include "camera.h"
#include <StopWatch.h>
#include <math.h>
#define NEAR_ZERO 0.05
#define WHEEL_ROTN_STEP 0.01
using namespace std;

void Car::calculateCircle()
{
	float thetaCar = m3dDegToRad(carRotationAngle);

	// calculating new circle of rotation
	float prev_radius = radius;
	float thetaWheel = m3dDegToRad(wheelAngle);

	// preventing INF
	if (wheelAngle > -NEAR_ZERO && wheelAngle < NEAR_ZERO)
	{
		if (prev_radius > 0)
			radius = INFINITY;
		else
			radius = -INFINITY;
	}
	else
	{
		radius = wheelBase / tan(thetaWheel);
	}
	circleCenter.x = (prev_radius - radius) * cos(thetaCar) + circleCenter.x;
	circleCenter.z = circleCenter.z - ((prev_radius - radius) * sin(thetaCar));
}



void Car::updateAngles()
{
	mat3 rotn = Transform::rotate(wheelAngle, myCamera.up);

	calculateCircle();

	// turning only the front wheels left
	for (int i = 0; i < 2; i++)
	{
		frontWheel[i].rotate = mat4(rotn);
		frontWheel[i].transform = carBody[0].transform * frontWheel[i].translate * frontWheel[i].rotate;
	}
}
void Car::turnLeft(float dt)
{
	// limiting the rotaing of wheel and increasing wheelAngle
	if (wheelAngle < wheelTurnLimit)
	{
		wheelAngle = wheelAngle + wheelStep * dt;
	}
}

void Car::turnRight(float dt)
{

	// limiting the rotaing of wheel and decreasing wheelAngle
	if (wheelAngle > -wheelTurnLimit)
	{
		wheelAngle = wheelAngle - wheelStep * dt;
	}
}


float signum(float x)
{
	if (x < 0)
		return -1;
	else
		return 1;
}

void Car::updatePosition()
{
	static CStopWatch timer;
	// the time elapsed since last frame
	float time = timer.GetElapsedSeconds();
	timer.Reset();	// reset the timer so that in next frame we get time elapsed

	// set constant acceleration if the car is accelerating (user has pressed the up key)
	float acceleration;
	if (accelerating)
	{
		if (currentGear == 1)
			acceleration = accelerationGear1;
		if (currentGear == 2)
			acceleration = accelerationGear2;
	}
	else
		acceleration = 0;

	if (de_accelerating)
	{
		if (currentGear == 1)
			acceleration = -accelerationGear1;

		if (currentGear == 2)
			acceleration = -accelerationGear2;
	}

	if (turningLeft == false && turningRight == false && (wheelAngle > NEAR_ZERO || wheelAngle < -NEAR_ZERO))
	{
		const float alpha = 0.01;

		// if time is greated than 1/40 fps then problem if large value of time is substituted in d_alpha
		if (time > 0.025)
			time = 0.025;

		float d_alpha = alpha * time * linearVelocity;

		wheelAngle = wheelAngle * (1 - d_alpha);
	}

	else
	{
		if (turningLeft)
			turnLeft(time);
		if (turningRight)
			turnRight(time);
	}

	// need to update variuous angles based on wheel angle
	updateAngles();

	linearVelocity = linearVelocity + (acceleration * time);


	float decreaseInVelocity = (constFriction + proportionateFriction * linearVelocity * linearVelocity) * time;

	if (decreaseInVelocity > linearVelocity) {
		linearVelocity = 0;
	}

	else {
		linearVelocity = (abs(linearVelocity) - decreaseInVelocity) * signum(linearVelocity);
	}

	float angularVelocity = linearVelocity / abs(radius);

	mat3 rotn;
	// moving forward while turning left
	float angleStep = 0;
	angleStep = (angularVelocity * time);

	//cout << "\n" << wheelAngle;

	// to put car in reverse, negate the angle
	if (reverseGear == true || colliding == true && collidingVelocity > 20)
	{
		angleStep = -angleStep;
	}

	// if colliding in reverse gear the should rebound , and if reversing when stuck reverse should reverse the car
	if (colliding == true && reverseGear == true && collidingVelocity > 20)
	{
		angleStep = -angleStep;
	}

	if (wheelAngle >= 0)
	{
		carRotationAngle = carRotationAngle + angleStep;
		rotn = Transform::rotate(angleStep, myCamera.up);
	}

	// moving forward while turning right
	if (wheelAngle < 0)
	{
		carRotationAngle = carRotationAngle - angleStep;
		rotn = Transform::rotate(-angleStep, myCamera.up);
	}

	//cout << "radius: " << radius << " thetaWheel " << wheelAngle << " angle: " << carRotationAngle << " anglestep: " << angleStep << "\n";	

	// traslating origin to change circleCenter as car rotates about the circle of rotation 	
	vec4 positionNew;
	positionNew = carBody[0].position - circleCenter;
	positionNew = mat4(rotn) * positionNew;

	carBody[0].position = positionNew + circleCenter;

	// compute rotn of the car
	mat3 rotnCar = Transform::rotate(carRotationAngle, myCamera.up);

	// rotaring the wheel as they roll on land
	mat3 rotnWheel = Transform::rotate(wheelRevolveAngle, vec3(1, 0, 0));

	// TODO : change this so that this is linked with speed of car
	// faster the car moves, faster the wheels rolls/ revolves 
	wheelRevolveAngle = wheelRevolveAngle - linearVelocity * WHEEL_ROTN_STEP;


	// calculating tranlation of the car, by seeing its position
	mat4 transl = Transform::translate(carBody[0].position.x, carBody[0].position.y, carBody[0].position.z);

	//cout << " \n " << carBody[0].position.x << "            and y is" << carBody[0].position.z;

	// actually affecting transform of the car
	carBody[0].transform = transl * mat4(rotnCar) * carBody[0].scale;
	carBody[1].transform = transl * mat4(rotnCar) * carBody[0].scale;

	// transform the 4 wheels
	frontWheel[0].transform = carBody[0].transform * frontWheel[0].translate * frontWheel[0].rotate * mat4(rotnWheel);
	frontWheel[1].transform = carBody[0].transform * frontWheel[1].translate * frontWheel[1].rotate  * mat4(rotnWheel);

	rearWheel[0].transform = carBody[0].transform * rearWheel[0].translate * mat4(rotnWheel);
	rearWheel[1].transform = carBody[0].transform * rearWheel[1].translate * mat4(rotnWheel);

	// rotate the third person camera like you rotated the car
	myCamera.rotateCamera(circleCenter, rotn, carCentre);
}

void Car::respondToCollision()
{
	// the velocity is lost in collision as non elastic collision
	collidingVelocity = linearVelocity / 2;
	colliding = true;

	myCar.accelerating = false;
	myCar.de_accelerating = true;

}

void Car::initializeCar()
{
	float amb[4] = { 1, 1, 1, 1 };
	float diff[4] = { 0.5, 0.5, 0.5, 1 };
	float spec[4] = { 1, 1, 1, 1 };
	float shine = 120;
	mat4 transl(1);

	/*
	0.08, 0, 0, 0,
	0, 0.08, 0, 0,
	0, 0, 0.08, 0,
	0, 0, 0, 1
	*/
	mat4 scale = mat4(
		0.08, 0, 0, 0,
		0, 0.08, 0, 0,
		0, 0, 0.08, 0,
		0, 0, 0, 1
		);

	myCar.carBody[0].initializeModelFromObjectFile("carExt.obj", "rose.tga", amb, diff, spec, shine, transl, scale);

	scale = mat4(1);
	transl = Transform::translate(4, 2, -6.3);
	myCar.frontWheel[0].initializeModelFromObjectFile("wheel.obj", "t1.tga", amb, diff, spec, shine, transl, scale);

	transl = Transform::translate(-4, 2, -6.3);
	myCar.frontWheel[1].initializeModelFromObjectFile("wheel.obj", "t1.tga", amb, diff, spec, shine, transl, scale);

	transl = Transform::translate(4, 2, 7);

	myCar.rearWheel[0].initializeModelFromObjectFile("wheel.obj", "t1.tga", amb, diff, spec, shine, transl, scale);

	transl = Transform::translate(-4, 2, 7);
	myCar.rearWheel[1].initializeModelFromObjectFile("wheel.obj", "t1.tga", amb, diff, spec, shine, transl, scale);

	amb[0] = 0.3;
	amb[1] = 0.1;
	amb[2] = 0.2;
	myCar.carBody[1].initializeModelFromObjectFile("carInt.obj", "sofa.tga", amb, diff, spec, shine, transl, scale);
}