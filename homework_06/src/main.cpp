#pragma 
#include <iomanip>
#include <iostream>
#include <sstream>
#include <span>

#include "ballistics.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: ballistic <input_path>\n";
        return 1;
    }

    auto args = std::span(argv, size_t(argc));

    BallisticParams params = readBallisticFile(args[1]);
    AmmoParams bomb = getAmmoParams(std::string(params.ammo_name.data()));

    float time = getTimeByCardano(bomb, params);
    std::cout << "time: " << time << '\n';

    float hDist = getDistance(bomb, params, time);
    std::cout << "distance: " << hDist << '\n';

    float Distance = std::sqrt((params.targetX - params.xd) * (params.targetX - params.xd) +
                        (params.targetY - params.yd) * (params.targetY - params.yd));
    std::stringstream sstr = {};
    if ((hDist + params.accelerationPath) > Distance) {
        float xdI =
            params.targetX - (params.targetX - params.xd) * (hDist + params.accelerationPath) / Distance;
        float ydI =
            params.targetY - (params.targetY - params.yd) * (hDist + params.accelerationPath) / Distance;
        Point pointI = {xdI, ydI};
        std::cout << "intermediate point: " << pointI.x << ", " << pointI.y << '\n';
        sstr << std::fixed << std::setprecision(3) << pointI.x << ", " << pointI.y;
    }

    float ratio = (Distance - hDist) / Distance;
    float fireX = params.xd + (params.targetX - params.xd) * ratio;
    float fireY = params.yd + (params.targetY - params.yd) * ratio;
    Point pointF = {fireX, fireY};
    std::cout << "fire point: " << pointF.x << ", " << pointF.y << '\n';

    sstr << " " << pointF.x << ", " << pointF.y;
    writeStringIntoFile(sstr.str());

    return 0;
}