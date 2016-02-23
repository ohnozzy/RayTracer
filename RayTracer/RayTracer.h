#pragma once
#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif
#include "resource.h"
#include "Transform.h"
EXTERN float tan_half_fov;
struct Ray {
	vec3 origin;
	vec3 direction;
};
struct Intersection {
	float t;
	unsigned index;
	vec3 direction;
	vec3 intersection;
	vec3 norm;
	vec3* ambient;
	vec3* diffuse;
	vec3* specular;
	vec3* emission;
	float shininess;
};

Intersection* Intersect(Ray ray, unsigned omit);
vec3 getColor(Intersection* hit);
vec3 raytrace(Ray& camray, unsigned n, unsigned omit);