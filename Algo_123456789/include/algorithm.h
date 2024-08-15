#ifndef ALGORITHM_123456789_H
#define ALGORITHM_123456789_H

#include "abstract_algorithm.h"
#include <vector>
#include <unordered_map>
#include "map_utility.h"
#include "enums.h"
#include <iostream>

class Algo_123456789 : public AbstractAlgorithm {
private:
    std::size_t max_steps = 0;
    const WallsSensor *wall_sensor_ptr = nullptr;
    const DirtSensor *dirt_sensor_ptr = nullptr;
    const BatteryMeter *battery_meter_ptr = nullptr;

public:
    void setMaxSteps(std::size_t max_steps_num) {
        max_steps = max_steps_num;
    }
	void setWallsSensor(const WallsSensor& wall_sensor){
        wall_sensor_ptr = &wall_sensor;
    }
	void setDirtSensor(const DirtSensor& dirt_sensor) {
        dirt_sensor_ptr = &dirt_sensor;
    }
	void setBatteryMeter(const BatteryMeter& battery_meter) {
        battery_meter_ptr = &battery_meter;
    }
	Step nextStep();
};

#endif