#ifndef SIMULATOR_DATA_H
#define SIMULATOR_DATA_H

#include "house.h"
#include "enums.h"
#include "map_utility.h"
#include <unordered_map>

class MySimulator;

class SimulatorData {

    SimulatorData() = delete;

    private:
    House& house;
    std::pair<int, int> vacuum_cleaner_coor;
    std::size_t vacuum_cleaner_battery;
    std::unordered_map<std::pair<int, int>, int, PairHasher, IntegerPairComparator> dirt_map;
    int dirt_count;

    SimulatorData(House& house);
    std::pair<int, int> static getAdvancedCoorByDirectionInHouse(std::pair<int, int> coor, Direction dir);
    bool applyStep(Step s);
    void charge();
    void moveVacuum(Direction d);
    void registerDirtData();
    int getDirtLevel(std::pair<int, int> coor);
    bool cleanDirt(std::pair<int, int> coor);

    size_t getMaxMissionSteps() {
        return house.getMaxMissionSteps();
    }

    bool isVacuumAtDocking() {
        return (house.getDockingStation() == vacuum_cleaner_coor);
    }

    int getTotalDirt() {
        return dirt_count;
    }

    public:
    
    std::size_t batteryMeterQuery() {
        return vacuum_cleaner_battery;
    }

    int dirtSensorQuery() {
        return getDirtLevel(vacuum_cleaner_coor);
    }

    bool wallSensorQuery(Direction d) {
        std::pair<int, int> temp_coor = getAdvancedCoorByDirectionInHouse(vacuum_cleaner_coor, d);
        return house.isWall(temp_coor.first, temp_coor.second);
    }

    friend MySimulator;
};

#endif