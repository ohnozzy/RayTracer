#include "stdafx.h"
#include "Transform.h"
#include "readfile.h"
#include "RayTracer.h"
#include "variables.h"
#include <algorithm>
vec3 raytrace(Ray& camray, unsigned n, unsigned omit) {
	Intersection* hit = Intersect(camray, omit);
	vec3 color = getColor(hit);
	if (n == 0|| hit==NULL) {
		delete hit;
		return color;
	}
	else {
		 Ray newRay;
		 newRay.origin = hit->intersection;
		 newRay.direction = -(Transform::rotate(180, hit->norm)*hit->direction);
		 vec3 mirror_color = raytrace(newRay, n - 1, hit->index);
		 color=color + *(hit->specular)*mirror_color;
		 delete hit;
		 return color;
	}
}
Intersection* Intersect(const Ray& ray, sphere& sph) {
	vec3 center(sph.center[0]/ sph.center[3], sph.center[1] / sph.center[3], sph.center[2] / sph.center[3]);
	vec4 origin_hom = sph.inverse*vec4(ray.origin[0], ray.origin[1], ray.origin[2], 1);
	vec4 direction_hom = sph.inverse*vec4(ray.direction[0], ray.direction[1], ray.direction[2], 0);
	vec3 origin = vec3(origin_hom[0], origin_hom[1], origin_hom[2]) / origin_hom[3];
	vec3 direction = vec3(direction_hom[0], direction_hom[1], direction_hom[2]);

	vec3 P0C= origin - center;
	float a= glm::dot(direction, direction);
	float b = 2*glm::dot(direction, P0C);
	float c = glm::dot(P0C, P0C)-sph.radius*sph.radius;
	float deter = b*b - 4 *a* c;
	if (deter < 0) {
		return NULL;
	}
	else {
		float deter_sqrt=sqrtf(deter);
		float t = (-b - deter_sqrt) / (2*a);
		if (t > 0) {
			Intersection* output = new Intersection;
			output->index = sph.index;
			vec3 P= ray.origin + t*ray.direction;
			vec3 P_tran = origin + t*direction;
			/*vec4 P_assert_hom = sph.forward*vec4(P_tran[0], P_tran[1], P_tran[2], 1);
			vec3 P_assert(P_assert_hom[0]/ P_assert_hom[3], P_assert_hom[1] / P_assert_hom[3], P_assert_hom[2] / P_assert_hom[3]);
			vec3 dif = P - P_assert;
			
			if (abs(dif[0])>0.001|| abs(dif[1])>0.001|| abs(dif[2])>0.001) {
				P_assert[0] = 1;
			}*/
			
			vec3 norm3 = center- P_tran;
			vec4 norm4 = glm::transpose(sph.inverse)*vec4(norm3[0], norm3[1], norm3[2], 0);
			output->norm = glm::normalize(vec3(norm4[0], norm4[1], norm4[2]));
			output->intersection = P;
			output->direction = ray.direction;
			output->ambient = &(sph.ambient);
			output->diffuse = &(sph.diffuse);
			output->emission = &(sph.emission);
			output->specular = &(sph.specular);
			output->shininess = sph.shininess;
			output->t = t;
			return output;
		}
		else {
			return NULL;
		}
	}
	
}
Intersection* Intersect(const Ray& ray, triangle& tri) {
	
	vec3 A(tri.point[0].x / tri.point[0].w, tri.point[0].y / tri.point[0].w, tri.point[0].z / tri.point[0].w);
	vec3 B(tri.point[1].x / tri.point[1].w, tri.point[1].y / tri.point[1].w, tri.point[1].z / tri.point[1].w);
	vec3 C(tri.point[2].x / tri.point[2].w, tri.point[2].y / tri.point[2].w, tri.point[2].z / tri.point[2].w);
	vec3 AB = B - A;
	vec3 AC = C - A;
	vec3 norm = glm::cross(AC, AB);
	vec3 tri_norm=glm::normalize(norm);
	float P1n = glm::dot(ray.direction, tri_norm);
	
	if (P1n != 0) {
		
		float t = glm::dot(A - ray.origin, tri_norm)/P1n;
		if (t > 0) {
			vec3 P = ray.origin + t*ray.direction;
			vec3 BC = C - B;
			vec3 CA = -AC;
			vec3 AP = P - A;
			vec3 BP = P - B;
			vec3 CP = P - C;
			vec3 norm1 = glm::cross(AB, AP);
			vec3 norm2 = glm::cross(BC, BP);
			vec3 norm3 = glm::cross(CA, CP);
			if (glm::dot(norm1, norm2) >= 0 && glm::dot(norm2, norm3) >= 0 && glm::dot(norm1, norm3) >= 0) {
				Intersection* output = new Intersection;
				
				if (P1n < 0) {
					output->norm = -tri_norm;
				}
				else {
					output->norm = tri_norm;
				}
				output->index = tri.index;
				output->intersection = P;
				output->direction = ray.direction;
				output->ambient = &(tri.ambient);
				output->diffuse = &(tri.diffuse);
				output->emission = &(tri.emission);
				output->specular = &(tri.specular);
				output->shininess = tri.shininess;
				output->t = t;
				
				return output;
			}
			else {
				return 0;
			}
		}else {
			return 0;
		}
	}else {
		return 0;
	}
}


Intersection* Intersect(Ray ray, unsigned omit) {
	unsigned num_tri=triangles.size();
	Intersection* best=NULL;
	for (unsigned i = 0; i < num_tri; i++) { // Find closest intersection; test all objects
		if (triangles[i].index != omit) {
			Intersection* point = Intersect(ray, triangles[i]);
			if (point != NULL && (best == NULL || point->t < best->t)) { // closer than previous closest object
				if (best != NULL)
					delete best;
				best = point;
			}
			else {
				delete point;
			}
		}
	}
	unsigned num_spheres = spheres.size();
	for (unsigned i = 0; i < num_spheres; i++) { // Find closest intersection; test all objects
		if (spheres[i].index != omit) {
			Intersection* point = Intersect(ray, spheres[i]);
			if (point != NULL && (best == NULL || point->t < best->t)) { // closer than previous closest object
				if (best != NULL)
					delete best;
				best = point;
			}
			else {
				delete point;
			}
		}
	}
	return best;
}
bool CheckIntersect(Ray ray, unsigned index, float w) {
	
	unsigned num_tri = triangles.size();
	for (unsigned i = 0; i < num_tri; i++) { // Find closest intersection; test all objects
		if (triangles[i].index != index) {
			Intersection* point = Intersect(ray, triangles[i]);
			if (point != NULL&&(point->t<1||w==0)) { // closer than previous closest object
				delete point;
				return true;
			}
			else {
				delete point;
			}
		}
	}
	unsigned num_spheres = spheres.size();
	for (unsigned i = 0; i < num_spheres; i++) { // Find closest intersection; test all objects
		if (spheres[i].index != index) {
			Intersection* point = Intersect(ray, spheres[i]);
			if (point != NULL&&(point->t<1||w==0)) { // closer than previous closest object
				
				delete point;
				return true;
			}
			else {
				delete point;
			}
		}
	}
	return false;
}
vec3 ComputeLight(const vec3& mypos, const vec3& eyedirn, const vec3& normal, const vec3& mydiffuse, const vec3& myspecular, const float myshininess, unsigned index) {
	vec3 retval = vec3(0, 0, 0);
	Ray lightray;
	lightray.origin = mypos;
	float length, length_2;
	float factor;
	for (unsigned i = 0; i<numused; i++) {
		vec3 direction;
		int base_index = 4 * i;
		vec3 light_vec(lightransf[base_index], lightransf[base_index+1], lightransf[base_index+2]);
		vec3 light_color(lightcolor[base_index], lightcolor[base_index + 1], lightcolor[base_index + 2]);
		if (lightransf[base_index+3]== 0) {
			direction = glm::normalize(light_vec);
			lightray.direction = -direction;

		}
		else {
			direction = glm::normalize(mypos- light_vec / lightransf[base_index + 3] );
			lightray.direction = light_vec / lightransf[base_index + 3] - mypos;
			length=glm::length(lightray.direction);
			length_2 = length*length;
		}
		
		
		float nDotL = glm::dot(normal, direction);
		if (nDotL<0&& mydiffuse[0] == 0 && mydiffuse[1]==1 && mydiffuse[2] == 0) {
			float r = mydiffuse[0];
		}
		if (nDotL>0&&!CheckIntersect(lightray, index, lightransf[base_index + 3])) {
			vec3 half_vector = glm::normalize(direction + eyedirn);
			
			vec3 lambert = mydiffuse  * nDotL;
			float nDotH = glm::dot(normal, half_vector);
			vec3 phong = myspecular  * pow(max(nDotH, 0.0f), myshininess);
			factor = 1;
			if (lightransf[base_index + 3] !=0) {
				factor = 1 / (c[0] + c[1] * length+ c[2] * length_2);
			}
			retval = retval + factor*(lambert + phong)*light_color;
		}
	}
	return retval;
}
vec3 getColor(Intersection* hit) {
	if (hit != NULL) {
		
	    vec3 col = ComputeLight(hit->intersection, hit->direction, hit->norm, *(hit->diffuse), *(hit->specular), hit->shininess, hit->index);
		return *(hit->ambient) + *(hit->emission) + col;
	}
	else {
		return vec3(0, 0, 0);
	}
}
