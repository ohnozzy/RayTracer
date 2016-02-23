// Readfile definitions 
#pragma once
#include "stdafx.h"
#include "Transform.h"
#include <stack>
using namespace std;
void matransform(stack<mat4> &transfstack, float * values);
void matransform(const mat4 &transform, const float* values, float* newvalues);
void rightmultiply(const mat4 & M, stack<mat4> &transfstack);
bool readvals(stringstream &s, const int numvals, float * values);
void readfile (const WCHAR* filename) ;
