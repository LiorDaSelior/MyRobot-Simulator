#include "simulator_data.h"

SimulatorData::SimulatorData(House& house) : house(house) {
    reset();
}

void SimulatorData::reset() {
    dirt_map.clear();
    dirt_count = 0;
    vacuum_cleaner_coor = house.getDockingStation();
    vacuum_cleaner_max_battery = static_cast<double>(house.getMaxBatterySteps());
    vacuum_cleaner_battery = vacuum_cleaner_max_battery;
    current_charge = vacuum_cleaner_max_battery / 20;
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

bool SimulatorData::applyStep(Step s) { // false => step is useless, something is wrong in algorithm
    switch (s)
    {
    case Step::Finish:
        return true;
    case Step::Stay:
        if (getDirtLevel(vacuum_cleaner_coor) > 0) {
            if (vacuum_cleaner_battery <= 0)
                return false;
            vacuum_cleaner_battery--;
            cleanDirt(vacuum_cleaner_coor);
            return true;
        }
        else if (isVacuumAtDocking())
        {
            return charge();
        }
        return false;
    default: // got a direction
        if (vacuum_cleaner_battery <= 0)
            return false;
        vacuum_cleaner_battery--;
        moveVacuum((Direction)s);
        return true;
    }
}

bool SimulatorData::charge() { //TODO how to raise battery
    if (vacuum_cleaner_battery >= vacuum_cleaner_max_battery) {
        return false;
    }
    //std::cout << "decided to charge: " << std::fixed << current_charge << "\n";
    //std::cout << "prev battery: " <<  std::fixed << vacuum_cleaner_battery  << "\n";
    vacuum_cleaner_battery += current_charge;
    vacuum_cleaner_battery = std::min(vacuum_cleaner_battery, vacuum_cleaner_max_battery);
    //std::cout << "new battery: " << std::fixed << vacuum_cleaner_battery  << "\n";
    return true;
}

void SimulatorData::moveVacuum(Direction d) {
    std::pair<int, int> temp_coor = getAdvancedCoorByDirectionInHouse(vacuum_cleaner_coor, d);
    int x = temp_coor.first;
    int y = temp_coor.second;
    if (x >= 0 && x < static_cast<int>(house.houseMap.size()) && 
        y >= 0 && y < static_cast<int>(house.houseMap[0].size()) &&
        !house.isWall(x,y))
        vacuum_cleaner_coor = temp_coor;
    //std::cout << "doc loc: (" << house.getDockingStation().first << "," << house.getDockingStation().second << ")\n";
    //std::cout << "current vac loc: (" << vacuum_cleaner_coor.first << "," << vacuum_cleaner_coor.second << ")\n";
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