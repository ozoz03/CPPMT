#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES

#define LOG(msg) std::cout << "[LOG] " << msg << std::endl

const int TICKS_PER_REVOLUTION = 1024; //iмпульсiв на один оберт колеса
const double WHEEL_RADIUS_M = 0.3; //радiус колеса у метрах (дiаметр 60 см)
const double WHEELBASE_M = 1.0; //вiдстань мiж лiвим i правим бортом, у метрах

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
    double* d_left = new double[odometry_data.size() - 1];
    double* d_right = new double[odometry_data.size() - 1];
    double* distance_per_tick = new double[odometry_data.size() - 1];
    double* dL = new double[odometry_data.size() - 1];
    double* dR = new double[odometry_data.size() - 1];
    double* d = new double[odometry_data.size() - 1];
    double* dtheta = new double[odometry_data.size() - 1];

    Position position = {0.0, 0.0, 0.0}; // Початкове положення (x, y, theta)

    for (std::size_t i = 0; i < odometry_data.size(); ++i) {
        std::cout << odometry_data[i].timestamp_ms << " " << odometry_data[i].ticks.fl_ticks << " " << odometry_data[i].ticks.fr_ticks << " " << odometry_data[i].ticks.bl_ticks << " " << odometry_data[i].ticks.br_ticks << std::endl;
        if(i == 0) {
            LOG("First line read successfully, starting odometry calculations...");
            continue;
        }
        deltaByWheel[i] = calculateWheelDeltas(odometry_data[i], odometry_data[i - 1]); 
        d_left[i] = (deltaByWheel[i].ticks.fl_ticks + deltaByWheel[i].ticks.bl_ticks) / 2.0;
        d_right[i] = (deltaByWheel[i].ticks.fr_ticks + deltaByWheel[i].ticks.br_ticks) / 2.0;

        distance_per_tick[i] = (2 * M_PI * WHEEL_RADIUS_M) / TICKS_PER_REVOLUTION;
        dL[i] = d_left[i]  * distance_per_tick[i];
        dR[i] = d_right[i] * distance_per_tick[i];
        d[i]= (dL[i] + dR[i]) / 2.0;
        dtheta[i] = (dR[i] - dL[i]) / WHEELBASE_M;
        
        position.x += d[i] * cos(position.theta + dtheta[i] / 2);
        position.y += d[i] * sin(position.theta + dtheta[i] / 2);
        position.theta += dtheta[i];
        std::cout << "Position at " << deltaByWheel[i].timestamp_ms << ": x=" << position.x << " y=" << position.y << " theta=" << position.theta << std::endl;
    }

    delete[] deltaByWheel;
    deltaByWheel = nullptr;
    delete[] d_left;
    d_left = nullptr;
    delete[] d_right;
    d_right = nullptr;
    delete[] distance_per_tick;
    distance_per_tick = nullptr;
    delete[] dL;
    dL = nullptr;
    delete[] dR;
    dR = nullptr;
    delete[] d;
    d = nullptr;
    delete[] dtheta;
    dtheta = nullptr;

    // TODO: implement wheel odometry for a 4-wheel differential-drive UGV.
    //
    // Parameters:
    //   ticks_per_revolution = 1024
    //   wheel_radius_m       = 0.3
    //   wheelbase_m          = 1.0
    //
    // Input:  text file with 5 whitespace-separated numbers per line:
    //         timestamp_ms fl_ticks fr_ticks bl_ticks br_ticks
    // Output: same tabular format on stdout, starting from the second sample:
    //         timestamp_ms x y theta



    return 0;
}
