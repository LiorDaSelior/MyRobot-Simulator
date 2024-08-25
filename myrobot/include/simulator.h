#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "house.h"
#include "abstract_algorithm.h"
#include <string>
#include <list>
#include "battery_meter.h"
#include "dirt_sensor.h"
#include "wall_sensor.h"
#include "simulator_data.h"
#include <memory>


class MySimulator {
public:
    MySimulator() = delete;
    ~MySimulator() = default;
    MySimulator(const MySimulator& other) = delete; // using unique pointers
    MySimulator& operator=(const MySimulator&) = delete;

    MySimulator(House& house);
    void setAlgorithm(std::unique_ptr<AbstractAlgorithm> abstract_algorithm_pnt);
    void output(const std::string& algorithm_name);
    void run();
    int calculateScore();
    size_t getMaxSteps() {
        return simulator_data.getMaxMissionSteps();
    }
    int getInitDirt() {
        return simulator_data.getInitTotalDirt();
    }

private:
    std::string filename;
    SimulatorData simulator_data;
    std::unique_ptr<WallsSensor> wall_sensor;
    std::unique_ptr<DirtSensor> dirt_sensor;
    std::unique_ptr<BatteryMeter> battery_meter;
    std::unique_ptr<AbstractAlgorithm> algorithm;
    size_t step_count = 0;
    std::list<Step> step_list;
    bool is_finished = false;
    // TODO:
    // count and return score in run
    // optional output

    std::string getStatus();
};

#endif
