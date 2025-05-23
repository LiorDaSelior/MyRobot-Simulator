#ifndef HOUSE_H
#define HOUSE_H

#include <vector>
#include <string>
#include "enums.h"
#include <iostream>

class SimulatorData;
class SimulatorManager;

class House {
public:
    House() = default;
    House(const std::string& filename);
    std::string getHouseFilename() {
        return house_filename;
    }

private:
    std::string house_filename;
    std::vector<std::vector<int>> houseMap;
    std::vector<std::string> new_grid;
    std::pair<int, int> dockingStation;
    std::string file_name;
    std::size_t maxMissionSteps;
    std::size_t maxBatterySteps;
    int rows;
    int cols;
    int total_dirt;
    

    void cleanDirt(int x, int y);
    int getDirtLevel(int x, int y) const;
    bool isWall(int x, int y) const;
    std::pair<int, int> getDockingStation() const;
    std::size_t getMaxBatterySteps() const;
    std::size_t getMaxMissionSteps() const;
    int getRows() const;
    int getCols() const;
    bool isFullyCleaned() const;
    int getTotalRemainingDirt() const;
    void parseInputFile(const std::string& filename);
    void surroundWithWalls();
    std::string trim(const std::string& str);  
    int getTotalDirt() const {
        return total_dirt;
    }

friend SimulatorData;
friend SimulatorManager;
};

#endif // HOUSE_H
