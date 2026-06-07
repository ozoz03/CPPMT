#pragma once

#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>

struct AmmoParams {
    std::array<char, 32> name;
    float mass;
    float drag;
    float lift;
    unsigned int type;
};

struct Point {
    float x;
    float y;
};

struct BallisticParams {
    float xd;
    float yd;
    float zd;
    float targetX;
    float targetY;
    float attackSpeed;
    float accelerationPath;
    std::array<char, 32> ammo_name;
};

BallisticParams readBallisticFile(const std::string& filename);
AmmoParams getAmmoParams(const std::string& ammo_name);
float getTimeByCardano(const AmmoParams& bomb, const BallisticParams& params);
float getDistance(const AmmoParams& bomb, const BallisticParams& params, float time);
void writeStringIntoFile(const std::string& str);
std::array<char, 32> NextWord(std::ifstream& inputstream);