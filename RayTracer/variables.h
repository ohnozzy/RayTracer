/*****************************************************************************/
/* This is the program skeleton for homework 2 in CSE167x by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

// This is the basic include file for the global variables in the program.  
// Since all files need access to it, we define EXTERN as either blank or 
// extern, depending on if included in the main program or not.  
#pragma once

#ifdef MAINPROGRAM 
#define EXTERN 
#else 
#define EXTERN extern 
#endif 

EXTERN int amount; // The amount of rotation for each arrow press
EXTERN vec3 eye; // The (regularly updated) vector coordinates of the eye 
EXTERN vec3 up;  // The (regularly updated) vector coordinates of the up 

#ifdef MAINPROGRAM 
vec3 eyeinit(0.0,0.0,5.0) ; // Initial eye position, also for resets
vec3 upinit(0.0,1.0,0.0) ; // Initial up position, also for resets
vec3 center(0.0,0.0,0.0) ; // Center look at point 
int amountinit = 5;
int w = 600, h = 400 ; // width and height 
float fovy = 90.0 ; // For field of view
#else 
EXTERN vec3 eyeinit ; 
EXTERN vec3 upinit ; 
EXTERN vec3 center ; 
EXTERN int amountinit;
EXTERN int w, h ; 
EXTERN float fovy ; 
#endif 
EXTERN bool useGlu; // Toggle use of "official" opengl/glm transform vs user 
static enum {view, translate, scale} transop ; // which operation to transform 
enum shape {cube, teapot, tri} ;
EXTERN float sx, sy ; // the scale in x and y 
EXTERN float tx, ty ; // the translation in x and y

// Lighting parameter array, similar to that in the fragment shader
const int numLights = 10 ; 
EXTERN float lightposn [4*numLights] ; // Light Positions
EXTERN float lightcolor[4*numLights] ; // Light Colors
EXTERN float lightransf[4*numLights] ; // Lights transformed by modelview

EXTERN unsigned numused;                     // How many lights are used 
EXTERN unsigned maxverts;
EXTERN float* vertex;
EXTERN float* vertextranf;
EXTERN int vertexnum;
// Materials (read from file) 
// With multiple objects, these are colors for each.
EXTERN float ambient[3] ; 
EXTERN float diffuse[3] ; 
EXTERN float specular[3] ; 
EXTERN float emission[3] ; 
EXTERN float shininess ; 
EXTERN string outputfilename;
EXTERN unsigned maxdepth;

// For multiple objects, read from a file.  
const int maxobjects = 10 ; 
EXTERN int numobjects ;
EXTERN float c[3];
EXTERN struct object {
  shape type ; 
  float size ;
  float ambient[4] ; 
  float diffuse[4] ; 
  float specular[4] ;
  float emission[4] ; 
  float shininess ;
  mat4 transform ; 
} objects[maxobjects];

struct triangle {
	unsigned index;
	vec4 point[3];
	int vertex[3];
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
	mat4 transform;
} ;

struct sphere {
	unsigned index;
	vec4 center;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	float shininess;
	float radius;
	mat4 transform;
	mat4 forward;
	mat4 inverse;
};
struct Render_Range {
	unsigned min_i;
	unsigned max_i;
	unsigned min_j;
	unsigned max_j;
};
EXTERN vector<struct triangle> triangles;
EXTERN vector<struct sphere> spheres;
EXTERN mat4 modelview;