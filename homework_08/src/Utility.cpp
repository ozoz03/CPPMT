#include "BalisticResult.h"
#include "SimStep.h"
#include "AmmoParams.h"
#include "MissionConfig.h"
#include "Point.h"
#include "Target.h"
#include "TargetDistance.h"
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>
#include "Utility.h"
#include "MissionContext.h"
using json = nlohmann::json;

// ДЗ18: дистанції/кути від поточної позиції дрона до поточних позицій цілей.
std::vector<TargetDistance> calculateTargetDistances(const std::vector<Target>& targets, const SimStep& drone) {
	std::vector<TargetDistance> distances(targets.size(), {0, 0, 0});
	for (std::size_t i = 0; i < targets.size(); ++i) {
		float x = targets[i].pos.x;
		float y = targets[i].pos.y;
		float distance = std::sqrt(std::pow(drone.dronePos.x - x, 2) + std::pow(drone.dronePos.y - y, 2));
		double targetToDroneAngleRadians = std::atan2(y - drone.dronePos.y, x - drone.dronePos.x);
		distances[i] = {static_cast<int>(i), distance, targetToDroneAngleRadians};
	}
	return distances;
}

std::vector<float> getFlightTimeToTarget(std::vector<TargetDistance>& targetDistances, const MissionConfig& cfg) {
	std::vector<float> times;
	for (const auto& td : targetDistances) {
		times.push_back(td.distance / cfg.attackSpeed);
	}
	return times;
}

int getIndexOfMin(std::vector<float>& arr) {
	int minIndex = 0;
	for (std::size_t i = 1; i < arr.size(); i++) {
		if (arr[i] < arr[minIndex]) {
			minIndex = i;
		}
	}
	return minIndex;
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
		throw std::runtime_error("Error: Wrong model type.");
	}
	float phi=std::acos(arg);
	float t = 2*std::sqrt(-p/3) * std::cos((phi + 4*M_PI) / 3 ) - b / (3*a);
	return t;
}

float getDistanceByTime(const float& time, const AmmoParams& bomb, const MissionConfig& droneConfig) {
	float h = droneConfig.attackSpeed*time - time*time*bomb.drag*droneConfig.attackSpeed/(2*bomb.mass) +
	          + time*time*time*(6*bomb.drag*9.81*bomb.lift*bomb.mass - 6*bomb.drag*bomb.drag*(bomb.lift*bomb.lift-1)*droneConfig.attackSpeed)/(36*bomb.mass*bomb.mass) +
	          + pow(time, 4)* (-6*bomb.drag*bomb.drag*9.81*bomb.lift*(1+bomb.lift*bomb.lift+pow(bomb.lift,4))*bomb.mass + 3*bomb.drag*bomb.drag*bomb.drag*bomb.lift*bomb.lift*(1+bomb.lift*bomb.lift)*droneConfig.attackSpeed + 6*bomb.drag*bomb.drag*bomb.drag*pow(bomb.lift,4)*(1+bomb.lift*bomb.lift)*droneConfig.attackSpeed)  / (36*pow(1+bomb.lift*bomb.lift,2)*bomb.mass*bomb.mass*bomb.mass) +
	          + pow(time,5)*(3*bomb.drag*bomb.drag*bomb.drag*9.81*bomb.lift*bomb.lift*bomb.lift*bomb.mass - 3*pow(bomb.drag,4)*bomb.lift*bomb.lift*(1+bomb.lift*bomb.lift)*droneConfig.attackSpeed) / (36*(1+bomb.lift*bomb.lift)*pow(bomb.mass,4));
	return h;
}

BalisticResult getBalisticResult(const float time, const float distance, std::vector<Target>& targets,
	const SimStep& simStep, const MissionConfig& droneConfig) {
	// ДЗ18: майбутня позиція цілі — лінійна екстраполяція з поточної швидкості.
	const Target& tgt = targets[simStep.targetIdx];
	float predictedTargetX = tgt.pos.x + tgt.velocity.x * time;
	float predictedTargetY = tgt.pos.y + tgt.velocity.y * time;
	Point predictedTarget = {predictedTargetX, predictedTargetY};

	float D = std::sqrt((predictedTargetX - simStep.dronePos.x)*(predictedTargetX - simStep.dronePos.x) +
	                    (predictedTargetY - simStep.dronePos.y)*(predictedTargetY - simStep.dronePos.y));

	Point aimPoint{};
	if ((distance + droneConfig.accelPath) > D) {
		float xdI = predictedTargetX - (predictedTargetX - simStep.dronePos.x) * (distance + droneConfig.accelPath) / D;
		float ydI = predictedTargetY - (predictedTargetY - simStep.dronePos.y) * (distance + droneConfig.accelPath) / D;
		aimPoint = {xdI, ydI};
	}

	float ratio = (D - distance) / D;
	float fireX = simStep.dronePos.x + (predictedTargetX - simStep.dronePos.x) * ratio;
	float fireY = simStep.dronePos.y + (predictedTargetY - simStep.dronePos.y) * ratio;
	Point dropPoint = {fireX, fireY};

	return {dropPoint, aimPoint, predictedTarget};
}

float normalizeAngle(float angle) {
	return std::atan2(std::sin(angle), std::cos(angle));
}

float steeringRate(float delta, const MissionConfig& cfg) {
	float sign = (delta > 0.0f) ? 1.0f : -1.0f;
	return sign * std::min(cfg.angularSpeed, std::fabs(delta) / cfg.simTimeStep);
}

void writeDownJson(std::vector<SimStep> simSteps, int count) {
	json out;
	out["totalSteps"] = count;
	out["steps"] = json::array();
	for (int i = 0; i < count; i++) {
		json step;
		step["position"]        = {{"x", simSteps[i].dronePos.x}, {"y", simSteps[i].dronePos.y}};
		step["direction"]       = simSteps[i].droneDirection;
		step["state"]           = simSteps[i].droneStateName ? simSteps[i].droneStateName : "Undefined";
		step["targetIndex"]     = simSteps[i].targetIdx;
		step["dropPoint"]       = {{"x", simSteps[i].dropPoint.x},{"y", simSteps[i].dropPoint.y}};
		step["aimPoint"]        = {{"x", simSteps[i].aimPoint.x},{"y", simSteps[i].aimPoint.y}};
		step["predictedTarget"] = {{"x", simSteps[i].predictedTarget.x},{"y", simSteps[i].predictedTarget.y}};
		// ДЗ18: реальний час кроку — кроки нерівномірні (real-time потоки).
		step["timeSecSinceStart"] = simSteps[i].currentTime;
		out["steps"].push_back(step);
	}
	std::ofstream fout("simulation.json");
	fout << out.dump(2);
}

void writeOutputFile(MissionContext& ctx) {
	std::ofstream outFile("output.txt");
	if (outFile.is_open()) {
		outFile << "Circle count: " << ctx.currentStepIndex << std::endl;
		outFile << "Drone position: " <<  " (" << ctx.droneContext.dronePos.x << ", " << ctx.droneContext.dronePos.y << ", " << ctx.cfg.altitude << ")" << std::endl;
		outFile << "Drone direction: " << ctx.droneContext.droneDirection << " " << std::endl;
		outFile << "Drone state: " << ctx.droneContext.droneStateName << std::endl;
		outFile << "Current target: " <<  ctx.droneContext.targetIdx << std::endl;
		outFile.close();
	} else {
		std::cerr << "Error: Could not open the file." << std::endl;
	}
}

TargetDistance getNearestTarget(MissionContext& ctx, const std::vector<Target>& targets) {
	std::vector<TargetDistance> targetDistances = calculateTargetDistances(targets, ctx.droneContext);
	std::vector<float> targetDistanceTimes = getFlightTimeToTarget(targetDistances, ctx.cfg);

	// Додаємо орієнтовний час довороту (як у попередніх ДЗ).
	for (std::size_t i = 0; i < targets.size(); ++i) {
		targetDistanceTimes[i] += (ctx.turnRemaining * ctx.cfg.simTimeStep);
	}
	int nearestTargetIndex = getIndexOfMin(targetDistanceTimes);

	// Дистанція/курс — до поточної цілі (якщо ще не обрана, беремо найближчу).
	int cur = ctx.droneContext.targetIdx;
	if (cur < 0 || cur >= static_cast<int>(targets.size())) {
		cur = nearestTargetIndex;
	}
	ctx.droneContext.targetDistance = targetDistances[cur].distance;
	ctx.desiredDir = targetDistances[cur].angleToDroneRadians;

	return targetDistances[nearestTargetIndex];
}
