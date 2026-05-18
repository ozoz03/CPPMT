#include "../include/ballistics.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <cmath>
#define _USE_MATH_DEFINES


BallisticParams readBallisticFile(const std::string& filename){
    BallisticParams params;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
		exit(1);
    }
    	file >> params.xd;
		file >> params.yd;
		file >> params.zd;
		file >> params.targetX;
		file >> params.targetY;
		file >> params.attackSpeed;
		file >> params.accelerationPath;
		file >> params.ammo_name;
		
		file.close();
		std::cout << "Ballistic parameters read successfully from file: " << filename << std::endl;
		std::cout << "xd: " << params.xd << std::endl;
		std::cout << "yd: " << params.yd << std::endl;
		std::cout << "zd: " << params.zd << std::endl;
		std::cout << "targetX: " << params.targetX << std::endl;
		std::cout << "targetY: " << params.targetY << std::endl;
		std::cout << "attackSpeed: " << params.attackSpeed << std::endl;
		std::cout << "accelerationPath: " << params.accelerationPath << std::endl;
		std::cout << "ammo_name: " << params.ammo_name << std::endl;

    return params; 
}

AmmoParams getAmmoParams(const std::string& ammo_name) {
	const int VOG17=1;
	const int M67=2;
	const int RKG_3=3;
	const int GLIDING_VOG=4;
	const int GLIDING_RKG=5;

	std::map <std::string, int> ammo_name_mapping;

	ammo_name_mapping["VOG-17"]  = VOG17;
	ammo_name_mapping["M67"]    = M67;
	ammo_name_mapping["RKG-3"] = RKG_3;
	ammo_name_mapping["GLIDING-VOG"]  = GLIDING_VOG;
	ammo_name_mapping["GLIDING-RKG"]  = GLIDING_RKG;


	AmmoParams ammo;
	switch (ammo_name_mapping[ammo_name]) {
		case VOG17:
			ammo.mass=0.35;
			ammo.drag=0.07;
			ammo.lift=0.0;
			ammo.type = 0;
		break;

	case M67:
		ammo.mass=0.6;
		ammo.drag=0.10;
		ammo.lift=0.0;
		ammo.type = 0;
		break;

	case RKG_3:
		ammo.mass=1.2;
		ammo.drag=0.10;
		ammo.lift=0.0;
		ammo.type = 0;
		break;

	case GLIDING_VOG:
		ammo.mass=0.45;
		ammo.drag=0.10;
		ammo.lift=1.0;
		ammo.type=1;
		break;

	case GLIDING_RKG:
		ammo.mass=1.4;
		ammo.drag=0.10;
		ammo.lift=1.0;
		ammo.type=1;
		break;

	default:
		std::cerr << "Error: Wrong ammo_name." << std::endl;
		exit(1);
		break;
	}
	std::cout << "Ammo parameters for " << ammo_name << " retrieved successfully." << std::endl;
	std::cout << "mass: " << ammo.mass << std::endl;
	std::cout << "drag: " << ammo.drag << std::endl;
	std::cout << "lift: " << ammo.lift << std::endl;
	std::cout << "type: " << ammo.type << std::endl;
	return ammo;
}

float getTimeByCardano(const AmmoParams& bomb, const BallisticParams& params) {
	float a = bomb.drag*9.81*bomb.mass - 2*bomb.drag*bomb.drag*bomb.lift*params.attackSpeed;
	float b = -3*9.81*bomb.mass*bomb.mass + 3*bomb.drag*bomb.lift*bomb.mass*params.attackSpeed;
	float c = 6*bomb.mass*bomb.mass*params.zd;
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


float getDistance(const AmmoParams& bomb, const BallisticParams& params, float time) {
	float h = params.attackSpeed*time - time*time*bomb.drag*params.attackSpeed/(2*bomb.mass) +
	          + time*time*time*(6*bomb.drag*9.81*bomb.lift*bomb.mass - 6*bomb.drag*bomb.drag*(bomb.lift*bomb.lift-1)*params.attackSpeed)/(36*bomb.mass*bomb.mass) +
	          + pow(time, 4)* (-6*bomb.drag*bomb.drag*9.81*bomb.lift*(1+bomb.lift*bomb.lift+pow(bomb.lift,4))*bomb.mass + 3*bomb.drag*bomb.drag*bomb.drag*bomb.lift*bomb.lift*(1+bomb.lift*bomb.lift)*params.attackSpeed + 6*bomb.drag*bomb.drag*bomb.drag*pow(bomb.lift,4)*(1+bomb.lift*bomb.lift)*params.attackSpeed)  / (36*pow(1+bomb.lift*bomb.lift,2)*bomb.mass*bomb.mass*bomb.mass) +
	          + pow(time,5)*(3*bomb.drag*bomb.drag*bomb.drag*9.81*pow(bomb.lift,3)*bomb.mass - 3*pow(bomb.drag,4)*bomb.lift*bomb.lift*(1+bomb.lift*bomb.lift)*params.attackSpeed) / (36*(1+bomb.lift*bomb.lift)*pow(bomb.mass,4));
	return h;
}

void writeStringIntoFile(std::string str)
{
	std::ofstream outFile("output.txt");

	if (outFile.is_open()) {
		outFile <<  str << std::endl;

		outFile.close();
		std::cout << "File written successfully." << std::endl;
	} else {
		std::cerr << "Error: Could not open the file." << std::endl;
	}
}