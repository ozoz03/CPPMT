#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES

#define ENABLE_DEBUG  0

#define LOG(msg) std::cout << "[LOG] " << msg << std::endl

#if ENABLE_DEBUG
  #define DEBUG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
  #define DEBUG(msg)
#endif

const int TICKS_PER_REVOLUTION = 1024; 
const double WHEEL_RADIUS_M = 0.3; 
const double WHEELBASE_M = 1.0; 

struct Position {
    double x;
    double y;    
    double theta;    
};

struct WheelTicks {
    int fl_ticks; 
    int fr_ticks; 
    int bl_ticks; 
    int br_ticks; 
};

struct OdometryTick {
    long timestamp_ms;
    WheelTicks ticks;
};

OdometryTick parseLine(const std::string& line) {
    OdometryTick tick;
    std::stringstream ss(line);
    ss >> tick.timestamp_ms >> tick.ticks.fl_ticks >> tick.ticks.fr_ticks 
        >> tick.ticks.bl_ticks >> tick.ticks.br_ticks;
    return tick;
}

std::vector<OdometryTick> readOdometryData(const char* fname) {
    std::ifstream file(fname);
    std::vector<OdometryTick> data;
    std::string line;
    if (file.is_open()) {
        while (std::getline(file, line)) {
            data.push_back(parseLine(line));
        }
        file.close();
  
    } else {
        std::cerr << "Error opening file: " << fname << std::endl;
        exit(1);
    }
    return data;   
}

OdometryTick calculateWheelDeltas(const OdometryTick& current, const OdometryTick& previous) {
    OdometryTick delta;
    delta.timestamp_ms = current.timestamp_ms;
    delta.ticks.fl_ticks = current.ticks.fl_ticks - previous.ticks.fl_ticks;
    delta.ticks.fr_ticks = current.ticks.fr_ticks - previous.ticks.fr_ticks;
    delta.ticks.bl_ticks = current.ticks.bl_ticks - previous.ticks.bl_ticks;
    delta.ticks.br_ticks = current.ticks.br_ticks - previous.ticks.br_ticks;
    return delta;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: ugv_odometry <input_path>\n";
        return 1;
    }

    std::vector <OdometryTick> odometry_data = readOdometryData(argv[1]);
    OdometryTick* deltaByWheel = new OdometryTick[odometry_data.size() - 1];
    double d_left , d_right, distance_per_tick, dL, dR, d, dtheta;

    Position position = {0.0, 0.0, 0.0}; 

    for (std::size_t i = 0; i < odometry_data.size(); ++i) {
        DEBUG(odometry_data[i].timestamp_ms << " " << odometry_data[i].ticks.fl_ticks << " " << odometry_data[i].ticks.fr_ticks << " " << odometry_data[i].ticks.bl_ticks << " " << odometry_data[i].ticks.br_ticks);
        if(i == 0) {
            // LOG("First line read successfully, starting odometry calculations...");
            continue;
        }
        deltaByWheel[i] = calculateWheelDeltas(odometry_data[i], odometry_data[i - 1]); 
        d_left = (deltaByWheel[i].ticks.fl_ticks + deltaByWheel[i].ticks.bl_ticks) / 2.0;
        d_right = (deltaByWheel[i].ticks.fr_ticks + deltaByWheel[i].ticks.br_ticks) / 2.0;

        distance_per_tick = (2 * M_PI * WHEEL_RADIUS_M) / TICKS_PER_REVOLUTION;
        dL = d_left  * distance_per_tick;
        dR = d_right * distance_per_tick;
        d= (dL + dR) / 2.0;
        dtheta = (dR - dL) / WHEELBASE_M;
        
        position.x += d * cos(position.theta + dtheta / 2);
        position.y += d * sin(position.theta + dtheta / 2);
        position.theta += dtheta;
        std::cout << deltaByWheel[i].timestamp_ms << " " << position.x << " " << position.y << " " << position.theta << std::endl;
    }

    delete[] deltaByWheel;
    deltaByWheel = nullptr;

    return 0;
}
