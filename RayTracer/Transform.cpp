// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.
#include "stdafx.h"
#include "Transform.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
    
    // YOUR CODE FOR HW2 HERE
    // Please implement this.  Likely the same as in HW 1.
	vec3 n_axis = glm::normalize(axis);
	float radians = glm::radians(degrees);
	float cos_theta = cos(radians);
	float sin_theta = sin(radians);
	mat3 R = cos_theta*mat3() + (1 - cos_theta)*glm::outerProduct(n_axis, n_axis) + sin_theta*mat3(0, n_axis.z, -n_axis.y, -n_axis.z, 0, n_axis.x, n_axis.y, -n_axis.x, 0);
    return R;
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
    // YOUR CODE FOR HW2 HERE
    // Likely the same as in HW 1. 
	mat3 R = rotate(degrees, up);
	eye = R*eye;
	up = R*up;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
    // YOUR CODE FOR HW2 HERE 
    // Likely the same as in HW 1.  
	vec3 axis = glm::cross(eye, up);
	mat3 R = rotate(degrees, axis);
	eye = R*eye;
	up = R*up;
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
	vec3 w = glm::normalize(eye-center);
	vec3 u = glm::normalize(glm::cross(up, w));
	vec3 v = glm::cross(w, u);
	mat4 R = mat4(u.x, v.x, w.x, 0, u.y, v.y, w.y, 0, u.z, v.z, w.z, 0, 0, 0, 0, 1);
	mat4 T = mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, -eye.x, -eye.y, -eye.z, 1);
	return R*T;
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
    mat4 ret;
	float n_minus_f = zNear - zFar;
	float A = (zNear + zFar) / n_minus_f;
	float B = 2*zNear*zFar/ n_minus_f;
	float d = 1 / tan(glm::radians(fovy) / 2);
    // YOUR CODE FOR HW2 HERE
    // New, to implement the perspective transform as well.  
    return mat4(d/aspect,0,0,0, 0,d,0,0,  0,0,A,-1, 0,0,B,0);
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    // YOUR CODE FOR HW2 HERE
    // Implement scaling 
    return mat4(sx,0,0,0, 0,sy,0,0, 0,0,sz,0, 0,0,0,1);
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
	
    // YOUR CODE FOR HW2 HERE
    // Implement translation 
    return mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, tx,ty,tz, 1);
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
    vec3 x = glm::cross(up,zvec); 
    vec3 y = glm::cross(zvec,x); 
    vec3 ret = glm::normalize(y); 
    return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
