#include "simulator_data.h"

SimulatorData::SimulatorData(House& house) : house(house) {
    reset();
}

void SimulatorData::reset() {
    dirt_map.clear();
    dirt_count = 0;
    vacuum_cleaner_coor = house.getDockingStation();
    vacuum_cleaner_battery = house.getMaxBatterySteps();
    registerDirtData();
}

void SimulatorData::registerDirtData() {
    dirt_count = 0;
    dirt_map.clear();
    for (int i = 0; i < house.rows; ++i) {
        for (int j = 0; j < house.cols; ++j) {
            if (house.houseMap[i][j] > 0) {
                dirt_map[std::pair<int, int>(i,j)] = house.houseMap[i][j];
                dirt_count += house.houseMap[i][j];
            }
        }
    }
}

int SimulatorData::getDirtLevel(std::pair<int, int> coor) {
    if (dirt_map.contains(coor)) {
        return dirt_map[coor];
    }
    return 0;
}

bool SimulatorData::cleanDirt(std::pair<int, int> coor) {
    if (dirt_map.contains(coor)) {
        dirt_map[coor] = dirt_map[coor] - 1;
        dirt_count--;
        if (dirt_map[coor] <= 0) {
            dirt_map.erase(coor);
        }
        return true;
    }
    return false;
}

bool SimulatorData::applyStep(Step s) {
    switch (s)
    {
    case Step::Finish:
        return true;
    case Step::Stay:
        if (getDirtLevel(vacuum_cleaner_coor) > 0) {
            vacuum_cleaner_battery--;
            cleanDirt(vacuum_cleaner_coor);
            return true;
        }
        else if (vacuum_cleaner_coor == house.dockingStation)
        {
            charge();
            return true;
        }
        return false;
    default: // got a direction
        vacuum_cleaner_battery--;
        moveVacuum((Direction)s);
        return true;
    }
}

void SimulatorData::charge() { //TODO how to raise battery
    vacuum_cleaner_battery += std::max((unsigned int)(house.getMaxBatterySteps() / 20),(unsigned int)1);
    vacuum_cleaner_battery = std::min(house.getMaxBatterySteps(),vacuum_cleaner_battery);
}

void SimulatorData::moveVacuum(Direction d) {
    std::pair<int, int> temp_coor = getAdvancedCoorByDirectionInHouse(vacuum_cleaner_coor, d);
    int x = temp_coor.first;
    int y = temp_coor.second;
    if (x >= 0 && x < static_cast<int>(house.houseMap.size()) && y >= 0 && y < static_cast<int>(house.houseMap[0].size())) 
        vacuum_cleaner_coor = temp_coor;
}

std::pair<int, int> SimulatorData::getAdvancedCoorByDirectionInHouse(std::pair<int, int> coor, Direction dir) {
    int i = 0;
    int j = 0;
    switch (dir)
    {
    case Direction::North:
        i = -1;
        break;
    case Direction::East:
        j = 1;
        break;
    case Direction::South:
        i = 1;
        break;
    case Direction::West:
        j = -1;
        break;
    }
    return std::pair<int, int>(coor.first + i, coor.second + j);
}