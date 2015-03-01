#include <glm/glm.hpp>
#include <StopWatch.h>
#include "model.h"
#define INFINITY 10000
static const float wheelTurnLimit = 60.0f;
const float accelerationGear1 = 500;
const float accelerationGear2 = 2000;

static const float constFriction = 0.01;
static const float proportionateFriction = 0.0005;
static const float wheelStep = 100;


typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;

class Car
{
public:
	Model carBody[2];
	Model frontWheel[2];
	Model rearWheel[2];


	// TODO: make all of these local variables if possible

	int currentGear;
	float wheelAngle;
	float carRotationAngle;
	float wheelRevolveAngle;
	float wheelBase;

	float linearVelocity, collidingVelocity;
	float acceleration;

	vec3 carCentre;

	float radius;
	float init_x, init_z;
	vec4 circleCenter;


	void calculateCircle();
	void updateAngles();

	void updatePosition();

	void turnLeft(float dt);
	void turnRight(float dt);

	void respondToCollision();


	bool turningLeft;
	bool turningRight;
	bool accelerating;
	bool de_accelerating;
	bool colliding;
	bool reverseGear;

	Car::Car()
	{
		carRotationAngle = 0;
		wheelAngle = 0;
		wheelRevolveAngle = 0;
		radius = INFINITY;
		circleCenter = vec4(-radius, 0, 0, 1);
		acceleration = 0;
		linearVelocity = 0;
		de_accelerating = false;
		wheelBase = 20;
		colliding = false;
		reverseGear = false;
		currentGear = 1;
	};
	void initializeCar();
};
extern Car myCar;