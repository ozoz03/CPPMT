#include "DronePhase.h"
#include "SimStep.h"
#include "AmmoParams.h"
#include "MissionConfig.h"
#include "Point.h"
#include "Target.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <array>
#include <fstream>
#include "sstream"


std::array<float, 5> calculateTargetDistances(const float& t,Target** targets, SimStep* simStep, const MissionConfig& droneConfig, double targetToDroneAngleRadians[5]) {
	std::array<float, 5> distances;
	int idx = (int)floor(t / droneConfig.arrayTimeStep) % 60;
	int next = (idx + 1) % 60;
    
	float frac = (t - idx * droneConfig.arrayTimeStep) / droneConfig.arrayTimeStep;
	for (int i=0; i < 5; ++i) {
		float x = targets[i]->positions[idx].x + (targets[i]->positions[next].x - targets[i]->positions[idx].x) * frac;
		float y = targets[i]->positions[idx].y + (targets[i]->positions[next].y - targets[i]->positions[idx].y) * frac;
		distances[i] = std::sqrt(std::pow(simStep->pos.x - x, 2) + std::pow(simStep->pos.y - y, 2));
		targetToDroneAngleRadians[i] = std::atan2(y - simStep->pos.y, x - simStep->pos.x);
		simStep->direction = std::fmod(simStep->direction + 2 * M_PI, 2 * M_PI);
		std::cout << "dinstance for target [" << i << "] = " << distances[i] << std::endl;
	}
	return distances;
}

std::array<float, 5> getFlightTimeToTarget(std::array<float, 5>targetDistances, const MissionConfig& cfg) {
	std::array<float, 5> times;
	for (int i=0; i < 5; ++i) {
		times[i]= targetDistances[i] / cfg.attackSpeed;
		std::cout << "Flight time to target [" << i << "] = " << times[i] << std::endl;
	}
	return times;
}


int getIndexOfMin(std::array<float, 5>arr) {
	int minIndex = 0;
	for (int i = 1; i < 5; i++) {
		if (arr[i] < arr[minIndex]) {
			minIndex = i;
		}
	}
	return minIndex;
}

double getTurnTime(int targetIndex, SimStep* simStep, double targetAngle, double targetAngleDiff[5], const MissionConfig& droneConfig){
	double angleDiff = std::fmod(targetAngle - simStep->direction + 2 * M_PI, 2 * M_PI);
	if (angleDiff > M_PI) {
		angleDiff -= 2 * M_PI;
	}
	targetAngleDiff[targetIndex] = angleDiff;
	double turnTime = std::abs(angleDiff) / droneConfig.angularSpeed;
	std::cout << "Turning towards target " << targetIndex << ", angle difference: " << angleDiff << " radians, turn time: " << turnTime << " seconds" << std::endl;
	return turnTime;
}

void doTurn(double targetAngleDiff, SimStep* simStep, int targetIndex, const MissionConfig& droneConfig) {
	double turnDirection = (targetAngleDiff > 0) ? 1 : -1;
	double turnAmount = turnDirection * droneConfig.angularSpeed * droneConfig.simTimeStep;
	if (std::abs(turnAmount) > std::abs(targetAngleDiff)) {
		turnAmount = targetAngleDiff; 
	}
	simStep->direction += turnAmount;
	std::cout << "Turning towards target " << targetIndex << ", current drone angle: " << simStep->direction << " radians" << std::endl;
}

void doMove(SimStep* simStep, const MissionConfig& droneConfig) {
	simStep->pos.x = simStep->pos.x + droneConfig.attackSpeed * std::cos(simStep->direction) * droneConfig.simTimeStep;
	simStep->pos.y = simStep->pos.y + droneConfig.attackSpeed * std::sin(simStep->direction) * droneConfig.simTimeStep;
	std::cout << "Moving towards target, current drone position: (" << simStep->pos.x << ", " << simStep->pos.y << ")" << std::endl;
}

float getTimeByCardano(const AmmoParams& bomb, const MissionConfig& droneConfig) {
	float a = bomb.drag*9.81*bomb.mass - 2*bomb.drag*bomb.drag*bomb.lift*droneConfig.attackSpeed;
	float b = -3*9.81*bomb.mass*bomb.mass + 3*bomb.drag*bomb.lift*bomb.mass*droneConfig.attackSpeed;
	float c = 6*bomb.mass*bomb.mass*droneConfig.altitude;
	float p = (-b*b)/(3*a*a);
	float q = 2*b*b*b / (27*a*a*a) + c/a;
	float arg =3*q / (2*p) * std::sqrt(-3/p);
	if ((arg<-1) || (arg>1)) {
		std::cerr << "Error: Wrong model type." << std::endl;
		exit(1);
	}

	float phi=std::acos(arg);
	float t = 2*std::sqrt(-p/3) * std::cos((phi + 4*M_PI) / 3 ) - b / (3*a);
	std::cout << "a: " << a << std::endl;
	std::cout << "b: " << b << std::endl;
	std::cout << "c: " << c << std::endl;
	std::cout << "p: " << p << std::endl;
	std::cout << "q: " << q << std::endl;
	std::cout << "phi: " << phi << std::endl;
	return t;
}

float getDistanceByTime(const float& time, const AmmoParams& bomb, const MissionConfig& droneConfig)
{
	float h = droneConfig.attackSpeed*time - time*time*bomb.drag*droneConfig.attackSpeed/(2*bomb.mass) +
	          + time*time*time*(6*bomb.drag*9.81*bomb.lift*bomb.mass - 6*bomb.drag*bomb.drag*(bomb.lift*bomb.lift-1)*droneConfig.attackSpeed)/(36*bomb.mass*bomb.mass) +
	          + pow(time, 4)* (-6*bomb.drag*bomb.drag*9.81*bomb.lift*(1+bomb.lift*bomb.lift+pow(bomb.lift,4))*bomb.mass + 3*bomb.drag*bomb.drag*bomb.drag*bomb.lift*bomb.lift*(1+bomb.lift*bomb.lift)*droneConfig.attackSpeed + 6*bomb.drag*bomb.drag*bomb.drag*pow(bomb.lift,4)*(1+bomb.lift*bomb.lift)*droneConfig.attackSpeed)  / (36*pow(1+bomb.lift*bomb.lift,2)*bomb.mass*bomb.mass*bomb.mass) +
	          + pow(time,5)*(3*bomb.drag*bomb.drag*bomb.drag*9.81*bomb.lift*bomb.lift*bomb.lift*bomb.mass - 3*pow(bomb.drag,4)*bomb.lift*bomb.lift*(1+bomb.lift*bomb.lift)*droneConfig.attackSpeed) / (36*(1+bomb.lift*bomb.lift)*pow(bomb.mass,4));
	return h;
}

void calculateBalistics(const AmmoParams& bomb, Target** targets, SimStep* simStep, const MissionConfig& droneConfig) {
	float time = getTimeByCardano(bomb, droneConfig);
	std::cout << "Time of Flight: " << time << std::endl;

	float h = getDistanceByTime(time, bomb, droneConfig);
	std::cout << "h distance: " << h << std::endl;

	int idx = (int)floor(time / droneConfig.arrayTimeStep) % 60;
	int next = (idx + 1) % 60;
	float frac = (time - idx * droneConfig.arrayTimeStep) / droneConfig.arrayTimeStep;
	float predictedTargetX = targets[simStep->targetIdx]->positions[idx].x + (targets[simStep->targetIdx]->positions[next].x - targets[simStep->targetIdx]->positions[idx].x) * frac;

	float predictedTargetY = targets[simStep->targetIdx]->positions[idx].y + (targets[simStep->targetIdx]->positions[next].y - targets[simStep->targetIdx]->positions[idx].y) * frac;
	simStep->predictedTarget = {predictedTargetX, predictedTargetY};
	std::cout << "predicted target position: (" << predictedTargetX << ", " << predictedTargetY << ")";
	float D = std::sqrt( (predictedTargetX - simStep->pos.x)*(predictedTargetX - simStep->pos.x) + (predictedTargetY - simStep->pos.y)*(predictedTargetY - simStep->pos.y) );

	if ((h + droneConfig.accelPath) > D) {
		float xdI = predictedTargetX - (predictedTargetX - simStep->pos.x) * (h + droneConfig.accelPath) / D;
		float ydI = predictedTargetY - (predictedTargetY - simStep->pos.y) * (h + droneConfig.accelPath) / D;
		Point CoordI = {xdI, ydI};
		simStep->aimPoint = CoordI;
		std::cout << "intermediate Coord: " << CoordI.x << ", " << CoordI.y << std::endl;
	}

	float ratio = (D - h) / D;
	float fireX = simStep->pos.x + (predictedTargetX - simStep->pos.x) * ratio;
	float fireY = simStep->pos.y + (predictedTargetY - simStep->pos.y) * ratio;
	Point CoordF = {fireX, fireY};
	std::cout << "fire Coord: " << CoordF.x << ", " << CoordF.y << std::endl;
	simStep->dropPoint = CoordF;
}

void writeStringIntoFile(std::stringstream& s1, std::stringstream& s2, std::stringstream& s3, std::stringstream& s4, std::stringstream& s5) {
	std::ofstream outFile("output.txt");

	if (outFile.is_open()) {
		outFile << "Circle count: " << s1.str() << std::endl;
		outFile << "Drone position: " <<  s2.str() << std::endl;
		outFile << "Drone direction: " << s3.str() << std::endl;
		outFile << "Drone state: " << s4.str() << std::endl;
		outFile << "Current target: " <<  s5.str() << std::endl;
		outFile.close();
		std::cout << "File written successfully." << std::endl;
	} else {
		std::cerr << "Error: Could not open the file." << std::endl;
	}
}

std::string statusToString(int phase) {
	switch (phase) {
	case STOPPED:
		return "STOPPED";
	case ACCELERATING:
		return "ACCELERATING";
	case DECELERATING:
		return "DECELERATING";
	case TURNING:
		return "TURNING";
	case MOVING:
		return "MOVING";
	default:
		return "UNKNOWN";
	}
}
