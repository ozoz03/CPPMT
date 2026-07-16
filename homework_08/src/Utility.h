#pragma once
#include "BalisticResult.h"
#include "SimStep.h"
#include "Target.h"
#include "MissionConfig.h"
#include "AmmoParams.h"
#include "TargetDistance.h"
#include <vector>
#include "MissionContext.h"

// ДЗ18: цілі приходять знімком {pos, velocity}. Дистанції та кути рахуються від
// поточної позиції дрона до поточних позицій цілей (без масиву траєкторій).
std::vector<TargetDistance> calculateTargetDistances(const std::vector<Target>& targets, const SimStep& drone);

std::vector<float> getFlightTimeToTarget(std::vector<TargetDistance>& targetDistances, const MissionConfig& cfg);

int getIndexOfMin(std::vector<float>& arr);

float getTimeByCardano(const AmmoParams& bomb, const MissionConfig& droneConfig);

float getDistanceByTime(const float& time, const AmmoParams& bomb, const MissionConfig& droneConfig);

// Прогноз позиції цілі — лінійна екстраполяція pos + velocity * time.
BalisticResult getBalisticResult(const float time, const float distance, std::vector<Target>& targets, const SimStep& simStep, const MissionConfig& droneConfig);

float normalizeAngle(float angle);

// ДЗ18: кутова швидкість корекції курсу за похибкою delta, обмежена angularSpeed.
// За один крок планування виправляє щонайбільше angularSpeed*simTimeStep рад.
float steeringRate(float delta, const MissionConfig& cfg);

void writeDownJson(std::vector<SimStep> simSteps, int count);
void writeOutputFile(MissionContext& ctx);
TargetDistance getNearestTarget(MissionContext& ctx, const std::vector<Target>& targets);
