#pragma once
#include "MissionConfig.h"
#include <cstring>

class MissionBuilder {
    MissionConfig cfg{};
public:
    // ДЗ18: дефолти для нових полів, якщо їх немає у config.json.
    MissionBuilder() { cfg.physicsTimeStep = 0.01f; cfg.timeScale = 1.0f; }
    MissionBuilder& physicsTimeStep(float t)
        { cfg.physicsTimeStep=t; return *this; }
    MissionBuilder& timeScale(float t)
        { cfg.timeScale=t; return *this; }
    MissionBuilder& startAt(float x, float y)
        { cfg.startPos={x,y}; return *this; }
    MissionBuilder& altitude(float a)
        { cfg.altitude=a; return *this; }
    MissionBuilder& maxTargets(int n)
        { cfg.maxTargets=n; return *this; }
    MissionBuilder& initialDir(float d)
        { cfg.initialDir=d; return *this; }
    MissionBuilder& attackSpeed(float s)
        { cfg.attackSpeed=s; return *this; }
    MissionBuilder& accelPath(float a)
        { cfg.accelPath=a; return *this; }
    MissionBuilder& arrayTimeStep(float t)
        { cfg.arrayTimeStep=t; return *this; }
    MissionBuilder& simTimeStep(float t)
        { cfg.simTimeStep=t; return *this; }
    MissionBuilder& hitRadius(float r)
        { cfg.hitRadius=r; return *this; }
    MissionBuilder& angularSpeed(float s)
        { cfg.angularSpeed=s; return *this; }
    MissionBuilder& turnThreshold(float t)
        { cfg.turnThreshold=t; return *this; }
    MissionBuilder& ammo(const char* a)
        { strncpy(cfg.ammoName,a,31); return *this; }   
    MissionConfig build() { return cfg; }
};
