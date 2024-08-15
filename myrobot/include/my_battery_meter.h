#ifndef MY_BATTERY_METER_H
#define MY_BATTERY_METER_H

#include <cstddef>
#include "simulator_data.h"
#include "battery_meter.h"


class MyBatteryMeter : public BatteryMeter {
private:
    SimulatorData& simulator_data;
public:
    MyBatteryMeter(SimulatorData& simulator_data) : simulator_data(simulator_data) {}; 
	std::size_t getBatteryState() const;
};

#endif