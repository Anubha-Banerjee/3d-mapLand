#include <glm/glm.hpp>
typedef glm::mat3 mat3 ;

typedef glm::vec3 vec3; 
typedef glm::vec4 vec4; 

class Camera
{
public:
	vec3 eye;
	vec3 up;
	vec3 goal;

	void rotateCamera(vec4 circleCenter, mat3 rotn, vec3 carCentre);
};
extern Camera myCamera;