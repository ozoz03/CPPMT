#pragma once
#include "Point.h"
#include "SourceType.h"


struct MissionConfig {
	Point startPos;     	
	float altitude;     	
	float initialDir;   
	float attackSpeed;  
	float accelPath;    
	char  ammoName[32]; 	
	float arrayTimeStep;
	float simTimeStep; 
	float hitRadius; 
	float angularSpeed; 	
	float turnThreshold;
	Source loaderType;
	int maxTargets;	 
};
