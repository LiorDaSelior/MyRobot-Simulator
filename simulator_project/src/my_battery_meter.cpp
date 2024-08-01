#include "my_battery_meter.h"

std::size_t MyBatteryMeter::getBatteryState() const {
    return simulator_data.batteryMeterQuery();
}