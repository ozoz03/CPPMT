#include <iostream>
#include "ballistics.hpp"
#include "ballistics.cpp"
#include <sstream>
#include <iomanip>


int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: ballistic <input_path>\n";
        return 1;
    }

	BallisticParams params = readBallisticFile(argv[1]);
    AmmoParams bomb = getAmmoParams(params.ammo_name);

    float time = getTimeByCardano(bomb, params);
	std::cout << "time: " << time << std::endl;

    float h = getDistance(bomb, params, time);
	std::cout << "distance: " << h << std::endl;

	float D = std::sqrt( (params.targetX - params.xd)*(params.targetX - params.xd) + (params.targetY - params.yd)*(params.targetY - params.yd) );
	std::stringstream ss;
	if ((h + params.accelerationPath) > D) {
		float xdI = params.targetX - (params.targetX - params.xd) * (h + params.accelerationPath) / D;
		float ydI = params.targetY - (params.targetY - params.yd) * (h + params.accelerationPath) / D;
		Point pointI = {xdI, ydI};
		std::cout << "intermediate point: " << pointI.x << ", " << pointI.y << std::endl;
		ss << std::fixed << std::setprecision(3)<<pointI.x <<", " <<pointI.y;
	}

	float ratio = (D - h) / D;
	float fireX = params.xd + (params.targetX - params.xd) * ratio;
	float fireY = params.yd + (params.targetY - params.yd) * ratio;
	Point pointF = {fireX, fireY};
	std::cout << "fire point: " << pointF.x << ", " << pointF.y << std::endl;

	ss <<" "<<pointF.x<<", " <<pointF.y;
	writeStringIntoFile(ss.str());

    return 0;
}