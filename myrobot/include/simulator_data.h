#ifndef SIMULATOR_DATA_H
#define SIMULATOR_DATA_H

#include "house.h"
#include "enums.h"
#include "map_utility.h"
#include <unordered_map>
#include <cmath>

class MySimulator;

class SimulatorData {

    SimulatorData() = delete;

    private:
    House& house;
    std::pair<int, int> vacuum_cleaner_coor;
    double vacuum_cleaner_max_battery; // needed cause of double
    double vacuum_cleaner_battery;
    double current_charge;
    std::unordered_map<std::pair<int, int>, int, PairHasher, IntegerPairComparator> dirt_map;
    int dirt_count;
    int init_dirt_count;
    
    SimulatorData(House& house);
    std::pair<int, int> static getAdvancedCoorByDirectionInHouse(std::pair<int, int> coor, Direction dir);
    bool applyStep(Step s);
    bool charge();
    void moveVacuum(Direction d);
    void registerDirtData();
    int getDirtLevel(std::pair<int, int> coor);
    bool cleanDirt(std::pair<int, int> coor);
    void reset();

    size_t getMaxMissionSteps() { // used by simulator
        return house.getMaxMissionSteps();
    }

    bool isVacuumAtDocking() {
        return (house.getDockingStation() == vacuum_cleaner_coor);
    }

    int getTotalDirt() {
        return dirt_count;
    }

    int getInitTotalDirt() {
        return init_dirt_count;
    }

    public:
    
    std::size_t batteryMeterQuery() {
        return static_cast<size_t>(std::round(vacuum_cleaner_battery * 1'000'000) / 1'000'000);
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