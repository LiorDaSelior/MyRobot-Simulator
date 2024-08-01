#include "my_dirt_sensor.h"

int MyDirtSensor::dirtLevel() const {
    return simulator_data.dirtSensorQuery();
}