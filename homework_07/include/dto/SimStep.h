#pragma once
#include "Point.h"

struct SimStep {
	Point dronePos;          	
	float droneDirection;    	
	const char* droneStateName;
	float droneSpeed;        	
	int   targetIdx;    	
	Point dropPoint;    	
	Point aimPoint;     	
	Point predictedTarget;  
};