#include "house.h"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cctype>
#include <map>


House::House(const std::string& filename) {
    try {
        parseInputFile(filename);
        //std::cout << "Completed parseInputFile without errors." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error in House constructor: " << e.what() << std::endl;
        throw;
    }
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
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open input file.");
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
            //std::cout << "File name: " << file_name << std::endl;
            firstLine = false;
            continue;
        }

        if (!readingGrid) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string variableName = trim(line.substr(0, pos));
                int value = std::stoi(trim(line.substr(pos + 1)));
                values[variableName] = value;
                //std::cout << variableName << " = " << value << std::endl;
            } else {
                // Start reading the grid
                readingGrid = true;
            }
        }

        if (readingGrid) {
            //need to check if the current house rows < given rows
            grid.push_back(line);
        }
    }

    if (grid.empty()) {

        throw std::runtime_error("No house found.");

    } 

    //we can brake on the while loop the moment we reach the house, and initialize the grid with rows and cols immediately
    file.close();
    //std::cout << "printing current house" << std::endl;
    //std::cout << "grid.size : " << grid.size() << std::endl;
    //std::cout << "grid.size[0] : " << grid[0].size() << std::endl;

    for(long unsigned int i=0; i<grid.size(); i++){
        for(long unsigned int j=0; j<grid[i].size(); j++){
            //std::cout << grid[i][j];
        }
        //std::cout << std::endl;
    }

    // Set the extracted values to class members
    if (values.find("MaxSteps") != values.end()) {
        maxMissionSteps = values["MaxSteps"];
    } else {
        throw std::runtime_error("Invalid house file: MaxSteps missing.");
    }
    if (values.find("MaxBattery") != values.end()) {
        maxBatterySteps = values["MaxBattery"];
    } else {
        throw std::runtime_error("Invalid house file: MaxBattery missing.");
    }
    if (values.find("Rows") != values.end()) {
        rows = values["Rows"];
    } else {
        throw std::runtime_error("Invalid house file: Rows missing.");
    }
    if (values.find("Cols") != values.end()) {
        cols = values["Cols"];
    } else {
        throw std::runtime_error("Invalid house file: Cols missing.");
    }

    new_grid.assign(rows, std::string(cols, ' '));

    //std::cout << "updated the house to the given rows/cols" << std::endl;
    //std::cout << "new_grid.size : " << new_grid.size() << std::endl;
    //std::cout << "new_grid.size[0] : " << new_grid[0].size() << std::endl;

    for(long unsigned int i=0; i<new_grid.size(); i++){
        for(long unsigned int j=0; j<new_grid[i].size(); j++){
            if(i<grid.size() && j < grid[i].size()){
                new_grid[i][j] = grid[i][j];
                //std::cout << new_grid[i][j];
            }
            else{
                new_grid[i][j] = ' ';
                //std::cout << new_grid[i][j];
            }
        }
        //std::cout << std::endl;
    }
    surroundWithWalls();
    // Initialize houseMap with zeros (without additional rows and columns for walls)
    houseMap.assign(rows, std::vector<int>(cols, 0));
    //std::cout << "Initialized houseMap with size " << rows << "x" << cols << std::endl;

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
        throw std::runtime_error("Invalid house file: Multiple or no docking stations found.");
    }

    // Print houseMap for debugging
    //std::cout << "Parsed houseMap:" << std::endl;
    for (size_t i = 0; i < houseMap.size(); ++i) {
        //std::cout << "Row " << i << ": ";
        for (size_t j = 0; j < houseMap[i].size(); ++j) {
            //std::cout << houseMap[i][j] << " ";
        }
        //std::cout << std::endl;
    }
    //std::cout << "docking station = " << dockingStation.first << ", " << dockingStation.second << std::endl;
    //std::cout << "Exiting parseInputFile." << std::endl;
}
void House::surroundWithWalls(){
    //std::cout << "surroundWalls" << std::endl;
    //std::cout << std::endl;

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

    // Debugging statements to verify correct wall placement
    //std::cout << "After adding walls:" << std::endl;
    // for(long unsigned int i=0; i<new_grid.size(); i++){
    //     for(long unsigned int j=0; j<new_grid[i].size(); j++){
    //         std::cout << new_grid[i][j];
    //     }
    //     std::cout << std::endl;
    // }
}
