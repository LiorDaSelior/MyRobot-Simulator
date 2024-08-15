#include "my_wall_sensor.h"

bool MyWallsSensor::isWall(Direction d) const {
    return simulator_data.wallSensorQuery(d);
}