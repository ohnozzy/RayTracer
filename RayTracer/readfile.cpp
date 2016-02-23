/*****************************************************************************/
/* This is the program skeleton for homework 2 in CSE167x by Ravi Ramamoorthi */
/* Extends HW 1 to deal with shading, more transforms and multiple objects   */
/*****************************************************************************/

/*****************************************************************************/
// This file is readfile.cpp.  It includes helper functions for matrix 
// transformations for a stack (matransform) and to rightmultiply the 
// top of a stack.  These functions are given to aid in setting up the 
// transformations properly, and to use glm functions in the right way.  
// Their use is optional in your program.  
  

// The functions readvals and readfile do basic parsing.  You can of course 
// rewrite the parser as you wish, but we think this basic form might be 
// useful to you.  It is a very simple parser.

// Please fill in parts that say YOUR CODE FOR HW 2 HERE. 
// Read the other parts to get a context of what is going on. 
  
/*****************************************************************************/

// Basic includes to get this file to work.  
#include "stdafx.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Transform.h" 

using namespace std;
#include "variables.h" 
#include "readfile.h"

// You may not need to use the following two functions, but it is provided
// here for convenience

// The function below applies the appropriate transform to a 4-vector
void matransform(stack<mat4> &transfstack, float* values) 
{
    mat4 transform = transfstack.top(); 
    vec4 valvec = vec4(values[0],values[1],values[2],values[3]); 
    vec4 newval = transform * valvec; 
    for (int i = 0; i < 4; i++) values[i] = newval[i]; 
}

void matransform(const mat4 &transform, const float* values, float* newvalues)
{
	
	vec4 valvec = vec4(values[0], values[1], values[2], values[3]);
	vec4 newval = transform * valvec;
	for (int i = 0; i < 4; i++) newvalues[i] = newval[i];
}

void rightmultiply(const mat4 & M, stack<mat4> &transfstack) 
{
    mat4 &T = transfstack.top(); 
    T = T * M; 
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.  
bool readvals(stringstream &s, const int numvals, float* values) 
{
    for (int i = 0; i < numvals; i++) {
        s >> values[i]; 
        if (s.fail()) {
            cout << "Failed reading value " << i << " will skip\n"; 
            return false;
        }
    }
    return true; 
}
bool readvals(stringstream &s, const int numvals, int* values)
{
	for (int i = 0; i < numvals; i++) {
		s >> values[i];
		if (s.fail()) {
			cout << "Failed reading value " << i << " will skip\n";
			return false;
		}
	}
	return true;
}

void readfile(const WCHAR* filename) 
{
    string str, cmd; 
    ifstream in;
    in.open(filename); 
	unsigned index = 1;
	maxdepth = 5;
	c[0] = 1;
	c[1] = 0;
	c[2] = 0;
    if (in.is_open()) {

        // I need to implement a matrix stack to store transforms.  
        // This is done using standard STL Templates 
        stack <mat4> transfstack; 
        transfstack.push(mat4(1.0));  // identity

        getline (in, str); 
        while (in) {
            if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
                // Ruled out comment and blank lines 

                stringstream s(str);
                s >> cmd; 
                int i; 
                float values[10]; // Position and color for light, colors for others
                                    // Up to 10 params for cameras.  
				int intvalues[10];
                bool validinput; // Validity of input 

                // Process the light, add it to database.
                // Lighting Command
                if (cmd == "light") {
                    if (numused == numLights) { // No more Lights 
                        cerr << "Reached Maximum Number of Lights " << numused << " Will ignore further lights\n";
                    } else {
                        validinput = readvals(s, 8, values); // Position/color for lts.
                        if (validinput) {							
                            // YOUR CODE FOR HW 2 HERE. 
                            // Note that values[0...7] shows the read in values 
                            // Make use of lightposn[] and lightcolor[] arrays in variables.h
                            // Those arrays can then be used in display too.
							unsigned start_offset = 4 * numused;
							unsigned end_offset = start_offset + 4;
							for (unsigned i = 0; i <4; i++) {
								lightposn[start_offset+i] = values[i];
								lightcolor[start_offset+i] = values[i + 4];
							}
							
							numused++;
							
                        }
                    }
				}
				else if (cmd == "point") {
					validinput = readvals(s, 6, values);
					if (validinput) {
						// YOUR CODE FOR HW 2 HERE. 
						// Note that values[0...7] shows the read in values 
						// Make use of lightposn[] and lightcolor[] arrays in variables.h
						// Those arrays can then be used in display too.
						unsigned start_offset = 4 * numused;
						memcpy(&lightposn[start_offset], values, 3 * sizeof(float));
						memcpy(&lightcolor[start_offset], &values[3], 3 * sizeof(float));
						lightposn[start_offset + 3] = 1;
						lightcolor[start_offset + 3] = 1;
						numused++;
					}

				}else if (cmd=="directional") {

					// Material Commands 
					// Ambient, diffuse, specular, shininess properties for each object.
					// Filling this in is pretty straightforward, so I've left it in 
					// the skeleton, also as a hint of how to do the more complex ones.
					// Note that no transforms/stacks are applied to the colors. 
					validinput = readvals(s, 6, values);
					if (validinput) {
						// YOUR CODE FOR HW 2 HERE. 
						// Note that values[0...7] shows the read in values 
						// Make use of lightposn[] and lightcolor[] arrays in variables.h
						// Those arrays can then be used in display too.
						unsigned start_offset = 4 * numused;
						for (unsigned i = 0; i <3; i++) {
							lightposn[start_offset + i] = -values[i];
							lightcolor[start_offset + i] = values[i + 3];
						}
						lightposn[start_offset + 3] = 0;
						lightcolor[start_offset + 3] = 1;
						numused++;

					}
				}else if (cmd == "ambient") {
                    validinput = readvals(s, 3, values); // colors 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            ambient[i] = values[i]; 
                        }
                    }
				}
				else if (cmd=="maxverts") {
					validinput = readvals(s, 1, intvalues);
					if (validinput) {
						maxverts = intvalues[0];
						vertex = new float[4*maxverts];
						vertextranf=new float[4 * maxverts];
						vertexnum = 0;
					}
				}
				else if (cmd == "vertex") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						unsigned start_offset = 4 * vertexnum;
						for (unsigned i = 0; i <3; i++) {
							vertex[start_offset + i] = values[i];							
						}
						vertex[start_offset + 3] = 1;
						vertexnum++;
					}
				}
				else if (cmd == "diffuse") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            diffuse[i] = values[i]; 
                        }
                    }
                } else if (cmd == "specular") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            specular[i] = values[i]; 
                        }
                    }
                } else if (cmd == "emission") {
                    validinput = readvals(s, 3, values); 
                    if (validinput) {
                        for (i = 0; i < 3; i++) {
                            emission[i] = values[i]; 
                        }
                    }
                } else if (cmd == "shininess") {
                    validinput = readvals(s, 1, values); 
                    if (validinput) {
                        shininess = values[0]; 
                    }
                } else if (cmd == "size") {
                    validinput = readvals(s,2,values); 
                    if (validinput) { 
                        w = (int) values[0]; h = (int) values[1]; 
                    } 
                } else if (cmd == "camera") {
                    validinput = readvals(s,10,values); // 10 values eye cen up fov
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE
                        // Use all of values[0...9]
                        // You may need to use the upvector fn in Transform.cpp
                        // to set up correctly. 
                        // Set eyeinit upinit center fovy in variables.h 
						for (unsigned i = 0; i < 3; i++) {
							eyeinit[i] = values[i];
							center[i] = values[i+3];
							upinit[i] = values[i + 6];
						}
						upinit=Transform::upvector(upinit, center - eyeinit);
						
						fovy = values[9];
                    }
                }

                // I've left the code for loading objects in the skeleton, so 
                // you can get a sense of how this works.  
                // Also look at demo.txt to get a sense of why things are done this way.
                else if (cmd == "cube" || cmd == "teapot") {
                    if (numobjects == maxobjects) { // No more objects 
                        cerr << "Reached Maximum Number of Objects " << numobjects << " Will ignore further objects\n";
                    } else {
                        validinput = readvals(s, 1, values); 
                        if (validinput) {
                            object * obj = &(objects[numobjects]); 
                            obj->size = values[0]; 

                            // Set the object's light properties
                            for (i = 0; i < 4; i++) {
                                (obj->ambient)[i] = ambient[i]; 
                                (obj->diffuse)[i] = diffuse[i]; 
                                (obj->specular)[i] = specular[i]; 
                                (obj->emission)[i] = emission[i];
                            }
                            obj->shininess = shininess; 

                            // Set the object's transform
                            obj->transform = transfstack.top(); 

                            // Set the object's type
                            if (cmd == "cube") {
                                obj->type = cube; 
                            } else if (cmd == "teapot") {
                                obj->type = teapot; 
                            }
                        }
                        ++numobjects; 
                    }
                }

                else if (cmd == "translate") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE.  
                        // Think about how the transformation stack is affected
                        // You might want to use helper functions on top of file. 
                        // Also keep in mind what order your matrix is!
						
						rightmultiply(Transform::translate(values[0], values[1], values[2]), transfstack);

                    }
				}else if (cmd=="tri") {
					validinput = readvals(s, 3, intvalues);
					if (validinput) {
						triangle tri;
						tri.index = index;
						index++;
						memcpy(tri.vertex, intvalues, 3 * sizeof(int));
						tri.ambient = vec3(ambient[0], ambient[1], ambient[2]);
						tri.emission = vec3(emission[0], emission[1], emission[2]);
						tri.diffuse = vec3(diffuse[0], diffuse[1], diffuse[2]);
						tri.specular = vec3(specular[0], specular[1], specular[2]);
						tri.shininess = shininess;
						for (unsigned i = 0; i < 3; i++) {
							tri.point[i] = transfstack.top()*vec4(vertex[4 * tri.vertex[i]], vertex[4 * tri.vertex[i] + 1], vertex[4 * tri.vertex[i] + 2], vertex[4 * tri.vertex[i] + 3]);
						}
						triangles.push_back(tri);
					}
				}
				else if (cmd=="sphere") {
					validinput = readvals(s, 4, values);
					if (validinput) {
						sphere sph;
						sph.index = index;
						index++;
						sph.center = vec4(values[0], values[1], values[2], 1);
						sph.ambient = vec3(ambient[0], ambient[1], ambient[2]);
						sph.emission = vec3(emission[0], emission[1], emission[2]);
						sph.diffuse = vec3(diffuse[0], diffuse[1], diffuse[2]);
						sph.specular = vec3(specular[0], specular[1], specular[2]);
						sph.shininess = shininess;
						sph.radius = values[3];
						sph.transform = transfstack.top();
						
						spheres.push_back(sph);
					}
				}else if (cmd == "scale") {
                    validinput = readvals(s,3,values); 
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE.  
                        // Think about how the transformation stack is affected
                        // You might want to use helper functions on top of file.  
                        // Also keep in mind what order your matrix is!

						rightmultiply(Transform::scale(values[0], values[1], values[2]), transfstack);
                    }
                }
                else if (cmd == "rotate") {
                    validinput = readvals(s,4,values); 
                    if (validinput) {

                        // YOUR CODE FOR HW 2 HERE. 
                        // values[0..2] are the axis, values[3] is the angle.  
                        // You may want to normalize the axis (or in Transform::rotate)
                        // See how the stack is affected, as above.  
                        // Note that rotate returns a mat3. 
                        // Also keep in mind what order your matrix is!
						
						rightmultiply(mat4(Transform::rotate(values[3], vec3(values[0], values[1], values[2]))), transfstack);
                    }
                }

                // I include the basic push/pop code for matrix stacks
                else if (cmd == "pushTransform") {
                    transfstack.push(transfstack.top()); 
                } else if (cmd == "popTransform") {
                    if (transfstack.size() <= 1) {
                        cerr << "Stack has no elements.  Cannot Pop\n"; 
                    } else {
                        transfstack.pop(); 
                    }
				}
				else if (cmd=="output") {
					s >> outputfilename;
				}
				else if (cmd == "maxdepth") {
					validinput = readvals(s, 1, intvalues);
					if (validinput) {
						maxdepth = intvalues[0];
					}
				}
				else if (cmd == "attenuation") {
					validinput = readvals(s, 3, values);
					if (validinput) {
						memcpy(c, values, 3 * sizeof(float));
					}
				}
				else {
                    cerr << "Unknown Command: " << cmd << " Skipping \n"; 
                }
            }
            getline (in, str); 
        }

        // Set up initial position for eye, up and amount
        // As well as booleans 

        eye = eyeinit; 
        up = upinit; 
        amount = 5;
        sx = sy = 1.0;  // keyboard controlled scales in x and y 
        tx = ty = 0.0;  // keyboard controllled translation in x and y  
        useGlu = false; // don't use the glu perspective/lookat fns

    } else {
        cerr << "Unable to Open Input Data File " << filename << "\n"; 
        throw 2; 
    }
	
}
