#pragma once
#include "SimStep.h"
#include <string>
#include "Target.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "sstream"
#include <vector>

std::string statusToString(int phase);

std::vector<float> calculateTargetDistances(const float& t,std::vector<Target>& targets, SimStep& simStep, const MissionConfig& droneConfig, std::vector<double>& targetToDroneAngleRadians); 

std::vector<float> getFlightTimeToTarget(std::vector<float>& targetDistances, const MissionConfig& cfg);

int getIndexOfMin(std::vector<float>& arr);

double getTurnTime(int targetIndex, const SimStep& simStep, double targetAngle, std::vector<double>& targetAngleDiff, const MissionConfig& droneConfig);

void doTurn(double targetAngleDiff, SimStep& simStep, int targetIndex, const MissionConfig& droneConfig);

void doMove(SimStep& simStep, const MissionConfig& droneConfig);

float getTimeByCardano(const AmmoParams& bomb, const MissionConfig& droneConfig);

float getDistanceByTime(const float& time, const AmmoParams& bomb, const MissionConfig& droneConfig);

void calculateBalistics(const AmmoParams& bomb, std::vector<Target>& targets, SimStep& simStep, const MissionConfig& droneConfig);

void writeStringIntoFile(std::stringstream& s1, std::stringstream& s2, std::stringstream& s3, std::stringstream& s4, std::stringstream& s5);

float normalizeAngle(float angle);