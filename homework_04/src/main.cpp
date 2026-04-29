#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#define LOG(msg) std::cout << "[LOG] " << msg << std::endl

const int ticks_per_revolution = 1024; //iмпульсiв на один оберт колеса
const double wheel_radius_m = 0.3; //радiус колеса у метрах (дiаметр 60 см)
const double wheelbase_m = 1.0; //вiдстань мiж лiвим i правим бортом, у метрах

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

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "usage: ugv_odometry <input_path>\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    std::string line;
    while (std::getline(file, line)) {
        OdometryTick tick = parseLine(line);
        LOG("Odometry import timestamp: " << tick.timestamp_ms << " fl_ticks: " << tick.ticks.fl_ticks 
        << " fr_ticks: " << tick.ticks.fr_ticks << " bl_ticks: " << tick.ticks.bl_ticks << " br_ticks: " << tick.ticks.br_ticks);
    }

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
