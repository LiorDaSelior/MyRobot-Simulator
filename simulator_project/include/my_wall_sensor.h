#ifndef MY_WALL_SENSOR_H
#define MY_WALL_SENSOR_H

#include "simulator_data.h"
#include "wall_sensor.h"
#include "enums.h"


class MyWallsSensor : public WallsSensor {
private:
    SimulatorData& simulator_data;
public:
    MyWallsSensor(SimulatorData& simulator_data) : simulator_data(simulator_data) {}; 
	bool isWall(Direction d) const;
};

#endif
