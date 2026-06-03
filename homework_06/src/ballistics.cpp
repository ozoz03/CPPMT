#include "ballistics.hpp"

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#define USE_MATH_DEFINES

std::array<char, 32> NextWord(std::ifstream& inputStream) {
    std::string nextWord;
    inputStream >> nextWord;
    std::array<char, 32> wordArray = {};
    std::strncpy(wordArray.data(), nextWord.c_str(), wordArray.size() - 1);
    wordArray.back() = '\0';
    return wordArray;
}

BallisticParams readBallisticFile(const std::string& filename) {
    BallisticParams params = {};
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << '\n';
        exit(1);
    }
    file >> params.xd;
    file >> params.yd;
    file >> params.zd;
    file >> params.targetX;
    file >> params.targetY;
    file >> params.attackSpeed;
    file >> params.accelerationPath;
    params.ammo_name = NextWord(file);

    file.close();
    std::cout << "Ballistic parameters read successfully from file: " << filename << '\n';
    std::cout << "xd: " << params.xd << '\n';
    std::cout << "yd: " << params.yd << '\n';
    std::cout << "zd: " << params.zd << '\n';
    std::cout << "targetX: " << params.targetX << '\n';
    std::cout << "targetY: " << params.targetY << '\n';
    std::cout << "attackSpeed: " << params.attackSpeed << '\n';
    std::cout << "accelerationPath: " << params.accelerationPath << '\n';
    std::cout << "ammo_name: " << params.ammo_name.data() << '\n';

    return params;
}

AmmoParams getAmmoParams(const std::string& ammo_name) {
    const int VOG17 = 1;
    const int M67 = 2;
    const int RKG_3 = 3;
    const int GLIDING_VOG = 4;
    const int GLIDING_RKG = 5;

    std::map<std::string, int> ammo_name_mapping;

    ammo_name_mapping["VOG-17"] = VOG17;
    ammo_name_mapping["M67"] = M67;
    ammo_name_mapping["RKG-3"] = RKG_3;
    ammo_name_mapping["GLIDING-VOG"] = GLIDING_VOG;
    ammo_name_mapping["GLIDING-RKG"] = GLIDING_RKG;

    AmmoParams ammo = {};
    switch (ammo_name_mapping[ammo_name]) {
        case VOG17:
            ammo.mass = 0.35;
            ammo.drag = 0.07;
            ammo.lift = 0.0;
            ammo.type = 0;
            break;

        case M67:
            ammo.mass = 0.6;
            ammo.drag = 0.10;
            ammo.lift = 0.0;
            ammo.type = 0;
            break;

        case RKG_3:
            ammo.mass = 1.2;
            ammo.drag = 0.10;
            ammo.lift = 0.0;
            ammo.type = 0;
            break;

        case GLIDING_VOG:
            ammo.mass = 0.45;
            ammo.drag = 0.10;
            ammo.lift = 1.0;
            ammo.type = 1;
            break;

        case GLIDING_RKG:
            ammo.mass = 1.4;
            ammo.drag = 0.10;
            ammo.lift = 1.0;
            ammo.type = 1;
            break;

        default:
            std::cerr << "Error: Wrong ammo_name." << '\n';
            exit(1);
            break;
    }
    std::cout << "Ammo parameters for " << ammo_name << " retrieved successfully." << '\n';
    std::cout << "mass: " << ammo.mass << '\n';
    std::cout << "drag: " << ammo.drag << '\n';
    std::cout << "lift: " << ammo.lift << '\n';
    std::cout << "type: " << ammo.type << '\n';
    return ammo;
}

float getTimeByCardano(const AmmoParams& bomb, const BallisticParams& params) {
    auto amg = static_cast<float>(bomb.drag * 9.81 * bomb.mass -
                                  2 * bomb.drag * bomb.drag * bomb.lift * params.attackSpeed);
    auto bii = static_cast<float>(-3 * 9.81 * bomb.mass * bomb.mass +
                                  3 * bomb.drag * bomb.lift * bomb.mass * params.attackSpeed);
    float cmass = 6 * bomb.mass * bomb.mass * params.zd;
    float pii = (-bii * bii) / (3 * amg * amg);
    float quu = 2 * bii * bii * bii / (27 * amg * amg * amg) + cmass / amg;
    float arg = (3 * quu / (2 * pii) * std::sqrt(-3 / pii));
    if ((arg < -1) || (arg > 1)) {
        std::cerr << "Error: Wrong model type." << '\n';
        exit(1);
    }

    float phi = std::acos(arg);
    auto time = static_cast<float>(2 * std::sqrt(-pii / 3) * std::cos((phi + 4 * M_PI) / 3) -
                                   bii / (3 * amg));
    // std::cout << "a: " << amg << '\n';
    // std::cout << "b: " << bii << '\n';
    // std::cout << "c: " << cmass << '\n';
    // std::cout << "p: " << pii << '\n';
    // std::cout << "q: " << quu << '\n';
    // std::cout << "phi: " << phi << '\n';
    return time;
}

float getDistance(const AmmoParams& bomb, const BallisticParams& params, float time) {
    auto distance = static_cast<float>(
        params.attackSpeed * time - time * time * bomb.drag * params.attackSpeed / (2 * bomb.mass) +
        +time * time * time *
            (6 * bomb.drag * 9.81 * bomb.lift * bomb.mass -
             6 * bomb.drag * bomb.drag * (bomb.lift * bomb.lift - 1) * params.attackSpeed) /
            (36 * bomb.mass * bomb.mass) +
        +pow(time, 4) *
            (-6 * bomb.drag * bomb.drag * 9.81 * bomb.lift *
                 (1 + bomb.lift * bomb.lift + pow(bomb.lift, 4)) * bomb.mass +
             3 * bomb.drag * bomb.drag * bomb.drag * bomb.lift * bomb.lift *
                 (1 + bomb.lift * bomb.lift) * params.attackSpeed +
             6 * bomb.drag * bomb.drag * bomb.drag * pow(bomb.lift, 4) *
                 (1 + bomb.lift * bomb.lift) * params.attackSpeed) /
            (36 * pow(1 + bomb.lift * bomb.lift, 2) * bomb.mass * bomb.mass * bomb.mass) +
        +pow(time, 5) *
            (3 * bomb.drag * bomb.drag * bomb.drag * 9.81 * pow(bomb.lift, 3) * bomb.mass -
             3 * pow(bomb.drag, 4) * bomb.lift * bomb.lift * (1 + bomb.lift * bomb.lift) *
                 params.attackSpeed) /
            (36 * (1 + bomb.lift * bomb.lift) * pow(bomb.mass, 4)));
    return distance;
}

void writeStringIntoFile(const std::string& str) {
    std::ofstream outFile("output.txt");

    if (outFile.is_open()) {
        outFile << str << '\n';

        outFile.close();
        std::cout << "File written successfully." << '\n';
    } else {
        std::cerr << "Error: Could not open the file." << '\n';
    }
}