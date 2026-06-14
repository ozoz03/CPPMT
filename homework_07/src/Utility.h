#pragma once
#include "SimStep.h"
#include <array>
#include <string>
#include "Target.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "sstream"

std::string statusToString(int phase);

std::array<float, 5> calculateTargetDistances(const float& t,Target** targets, SimStep* simStep, const MissionConfig& droneConfig, double targetToDroneAngleRadians[5]); 

std::array<float, 5> getFlightTimeToTarget(std::array<float, 5>targetDistances, const MissionConfig& cfg);

int getIndexOfMin(std::array<float, 5>arr);

double getTurnTime(int targetIndex, SimStep* simStep, double targetAngle, double targetAngleDiff[5], const MissionConfig& droneConfig);

void doTurn(double targetAngleDiff, SimStep* simStep, int targetIndex, const MissionConfig& droneConfig);

void doMove(SimStep* simStep, const MissionConfig& droneConfig);

float getTimeByCardano(const AmmoParams& bomb, const MissionConfig& droneConfig);

float getDistanceByTime(const float& time, const AmmoParams& bomb, const MissionConfig& droneConfig);

void calculateBalistics(const AmmoParams& bomb, Target** targets, SimStep* simStep, const MissionConfig& droneConfig);

void writeStringIntoFile(std::stringstream& s1, std::stringstream& s2, std::stringstream& s3, std::stringstream& s4, std::stringstream& s5);