#pragma once
#include "BalisticResult.h"
#include "SimStep.h"
#include "Target.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "TargetDistance.h"
#include "sstream"
#include <vector>
#include "MissionContext.h"

std::vector<TargetDistance> calculateTargetDistances(const float& t, const std::vector<Target>& targets, SimStep& simStep, const MissionConfig& droneConfig); 

std::vector<float> getFlightTimeToTarget(std::vector<TargetDistance>& targetDistances, const MissionConfig& cfg);

int getIndexOfMin(std::vector<float>& arr);

double getTurnTime(int targetIndex, const SimStep& simStep, double targetAngle, std::vector<double>& targetAngleDiff, const MissionConfig& droneConfig);

void doTurn(double targetAngleDiff, SimStep& simStep, int targetIndex, const MissionConfig& droneConfig);

void doMove(SimStep& simStep, const MissionConfig& droneConfig);

float getTimeByCardano(const AmmoParams& bomb, const MissionConfig& droneConfig);

float getDistanceByTime(const float& time, const AmmoParams& bomb, const MissionConfig& droneConfig);

BalisticResult calculateBalistics(const AmmoParams& bomb, std::vector<Target>& targets, const SimStep& simStep, const MissionConfig& droneConfig);

void writeStringIntoFile(std::stringstream& s1, std::stringstream& s2, std::stringstream& s3, std::stringstream& s4, std::stringstream& s5);

float normalizeAngle(float angle);

void writeDownJson(std::vector<SimStep> simSteps, int count);
void writeOutputFile(MissionContext& ctx);
TargetDistance getNearestTarget(MissionContext& ctx, const std::vector<Target>& targets);
