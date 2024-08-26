#include "house.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <map>
#include <custom_exception.h>

House::House(const std::string& filename) {
    house_filename = "unknown";
    try {
        parseInputFile(filename);
    } catch (SimulatorException& e) {
        throw;
    }
    catch (const std::exception& e) {
        throw HouseFailedToParseFile(house_filename, filename);
    }
    total_dirt = getTotalRemainingDirt();
}

void House::cleanDirt(int x, int y) {
    if (x >= 0 && x < static_cast<int>(houseMap.size()) && y >= 0 && y < static_cast<int>(houseMap[0].size())) {
        houseMap[x][y] = std::max(0, houseMap[x][y] - 1);
    }
}

int House::getDirtLevel(int x, int y) const {
    if (x >= 0 && x < static_cast<int>(houseMap.size()) && y >= 0 && y < static_cast<int>(houseMap[0].size())) {
        return houseMap[x][y];
    }
    return -1;
}

bool House::isWall(int x, int y) const {
    if (x >= 0 && x < static_cast<int>(houseMap.size()) && y >= 0 && y < static_cast<int>(houseMap[0].size())) {
        return houseMap[x][y] == -1;
    }
    return true;
}

std::pair<int, int> House::getDockingStation() const {
    return dockingStation;
}

std::size_t House::getMaxBatterySteps() const {
    return maxBatterySteps;
}

std::size_t House::getMaxMissionSteps() const {
    return maxMissionSteps;
}

int House::getRows() const {
    return static_cast<int>(houseMap.size());
}

int House::getCols() const {
    return static_cast<int>(houseMap[0].size());
}

bool House::isFullyCleaned() const {
    for (const auto& row : houseMap) {
        for (int cell : row) {
            if (cell > 0) {
                return false;
            }
        }
    }
    return true;
}

int House::getTotalRemainingDirt() const {
    int totalDirt = 0;
    for (const auto& row : houseMap) {
        for (int cell : row) {
            if (cell > 0) {
                totalDirt += cell;
            }
        }
    }
    return totalDirt;
}

std::string House::trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

void House::parseInputFile(const std::string& filename) {
    int index = filename.find_last_of('/');
    if (index == static_cast<int>(filename.npos))
        house_filename = filename;
    else
        house_filename = filename.substr(filename.find_last_of('/') + 1);
    house_filename = house_filename.substr(0,house_filename.find_last_of('.'));

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw HouseFailedToOpenFile(house_filename, filename);
    }
    std::string line;
    std::map<std::string, int> values;
    std::vector<std::string> grid;
    bool firstLine = true;
    bool readingGrid = false;

    while (std::getline(file, line)) {
        if (firstLine) {
            // First line is the file name
            file_name = line;
        }

        if (!readingGrid) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string variableName = trim(line.substr(0, pos));
                int value = std::stoi(trim(line.substr(pos + 1)));
                values[variableName] = value;
            } else {
                if (!firstLine)
                    readingGrid = true;
            }
        }

        if (readingGrid) {
            //need to check if the current house rows < given rows
            grid.push_back(line);
        }

        if (firstLine)
            firstLine = false;
    }

    if (grid.empty()) {
        throw HouseInvalidDockingStationAmountInFile(house_filename, filename, "0");
    } 

    //we can brake on the while loop the moment we reach the house, and initialize the grid with rows and cols immediately
    file.close();

    // Set the extracted values to class members
    if (values.find("MaxSteps") != values.end()) {
        maxMissionSteps = values["MaxSteps"];
    } else {
        throw HouseMissingAttributeInFile(house_filename, filename, "MaxSteps");
    }
    if (values.find("MaxBattery") != values.end()) {
        maxBatterySteps = values["MaxBattery"];
    } else {
        throw HouseMissingAttributeInFile(house_filename, filename, "MaxBattery");
    }
    if (values.find("Rows") != values.end()) {
        rows = values["Rows"];
    } else {
        throw HouseMissingAttributeInFile(house_filename, filename, "Rows");
    }
    if (values.find("Cols") != values.end()) {
        cols = values["Cols"];
    } else {
        throw HouseMissingAttributeInFile(house_filename, filename, "Cols");
    }

    new_grid.assign(rows, std::string(cols, ' '));

    for(long unsigned int i=0; i<new_grid.size(); i++){
        for(long unsigned int j=0; j<new_grid[i].size(); j++){
            if(i<grid.size() && j < grid[i].size()){
                new_grid[i][j] = grid[i][j];
            }
            else{
                new_grid[i][j] = ' ';
            }
        }
    }
    surroundWithWalls();
    // Initialize houseMap with zeros (without additional rows and columns for walls)
    houseMap.assign(rows, std::vector<int>(cols, 0));

    // Process the grid lines into the houseMap
    int num_of_dockstation = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            char ch = new_grid[i+1][j+1];
            if (ch == 'W') {
                houseMap[i][j] = -1;
            } else if (ch == 'D') {
                dockingStation = {static_cast<int>(i), static_cast<int>(j)};
                houseMap[i][j] = 0;
                num_of_dockstation++;
            } else if (ch == ' ') {
                houseMap[i][j] = 0;
            } else if (isdigit(ch)) {
                houseMap[i][j] = ch - '0';
            }
        }
    }
    if (num_of_dockstation != 1) {
        throw HouseInvalidDockingStationAmountInFile(house_filename, filename, std::to_string(num_of_dockstation));
    }

}
void House::surroundWithWalls(){
    if (rows+cols<=1) {
        return;
    }

    std::string first_row;
    first_row.assign(new_grid[0].size(), 'W');
    new_grid.insert(new_grid.begin(), first_row);
    
    std::string last_row;
    last_row.assign(new_grid[0].size(), 'W');
    new_grid.push_back(last_row);

    for (auto& row : new_grid) {
        row.insert(row.begin(), 'W');
    }

    for (auto& row : new_grid) {
        row.push_back('W');
    }
}
