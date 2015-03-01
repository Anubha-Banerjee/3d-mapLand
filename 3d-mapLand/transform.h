#include <glm/glm.hpp>

typedef glm::mat3 mat3 ;
typedef glm::mat4 mat4 ; 
typedef glm::vec3 vec3 ; 
typedef glm::vec4 vec4 ; 

class Transform  
{
public:
	Transform();
	virtual ~Transform();
	static void left(float degrees,vec3& eye, vec3& center, vec3& up, vec3& carCentre);
	static void up(float degrees, vec3& eye, vec3& up);
	static mat4 lookAt(vec3  eye,  vec3 up);
	static mat4 perspective(float fovy, float aspect, float zNear, float zFar);
    static mat3 rotate(const float degrees, const vec3& axis) ;
    static mat4 scale(const float &sx, const float &sy, const float &sz) ; 
    static mat4 translate(const float &tx, const float &ty, const float &tz);
    static vec3 upvector(const vec3 &up, const vec3 &zvec) ; 
};