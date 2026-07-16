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
	float physicsTimeStep; // ДЗ18: крок інтегрування фізики (менший за simTimeStep)
	float timeScale;       // ДЗ18: прискорення часу — сон = dt / timeScale
	float hitRadius;
	float angularSpeed; 	
	float turnThreshold;
	Source loaderType;
	int maxTargets;	 
};
