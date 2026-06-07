#pragma once
#include "Point.h"

struct SimStep {
	Point pos;          	
	float direction;    	
	int   state;        	
	int   targetIdx;    	
	Point dropPoint;    	
	Point aimPoint;     	
	Point predictedTarget;  
};