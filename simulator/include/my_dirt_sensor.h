#ifndef MY_DIRT_SENSOR_H
#define MY_DIRT_SENSOR_H

#include "simulator_data.h"
#include "dirt_sensor.h"

class MyDirtSensor : public DirtSensor{
private:
    SimulatorData& simulator_data;
public:
    MyDirtSensor(SimulatorData& simulator_data) : simulator_data(simulator_data) {};
	virtual int dirtLevel() const;
};

#endif