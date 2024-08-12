#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "abstract_algorithm.h"
#include <vector>
#include <unordered_map>
#include "map_utility.h"
#include "enums.h"
#include <iostream>

Direction getOppositeDirection(Direction dir);

std::pair<int, int> getAdvancedCoorByDirection(std::pair<int, int> coor, Direction dir);

class AnotherAlgorithm : public AbstractAlgorithm {
private:

    class InnerMap {
        private:

        class InnerMapCoordinatesInfo { // accessible only to InnerMap
            private:
            bool is_visited = false;
            bool is_dirty = true;
            std::vector<bool> is_wall_in_direction = std::vector<bool>(4, false); // N, E, S, W

            bool is_bfs_searched = false;
            Direction bfs_parent;
            size_t bfs_dist;


            public:
            void visit() {
                is_visited = true;
            }

            bool isVisited() {
                return is_visited;
            }

            void removeDirt() {
                is_dirty = false;
            }

            bool isDirty() {
                return is_dirty;
            }

            void addWall(Direction dir) {
                is_wall_in_direction[(int)dir] = true;
            }

            bool isWallInDirection(Direction dir) {
                return is_wall_in_direction[(int)dir];
            }

            void setBfsDistance(size_t dist) {
                bfs_dist = dist;
            }

            size_t getBfsDistance() {
                return bfs_dist;
            }

            void setBfsStatus(bool value) {
                is_bfs_searched = value;
            }

            bool isBfsSearched() {
                return is_bfs_searched;
            }

            void setBfsParent(Direction dir) {
                bfs_parent = dir;
            }

            Direction getBfsParent() {
                return bfs_parent;
            }
        };

        std::unordered_map<std::pair<int, int>, InnerMapCoordinatesInfo, PairHasher, IntegerPairComparator> graph_map;
        std::pair<int, int> closet_visited_and_dirty;
        bool closet_visited_and_dirty_flag;
        std::pair<int, int> closet_unvisited;
        bool closet_unvisited_flag;
        
        void resetBfsStatus();
        void bfsAlgorithm(std::pair<int, int> src_coor);

        public:
        InnerMap() {
            graph_map[(std::pair<int, int>(0,0))] = InnerMapCoordinatesInfo();
        }

        void bfs(std::pair<int, int> src_coor) {
            bfsAlgorithm(src_coor);
        }

        std::pair<int, int> getClosestUnvisited(std::pair<int, int> src_coor){
            return (closet_unvisited_flag) ? closet_unvisited : src_coor;
        }

        std::pair<int, int> getClosestDirty(std::pair<int, int> src_coor){
            return (closet_visited_and_dirty_flag) ? closet_visited_and_dirty : src_coor;
        }

        size_t getDistance(std::pair<int, int> dst_coor) {
            return graph_map[dst_coor].getBfsDistance();
        }; 

        Direction getDirection(std::pair<int, int> src_coor, std::pair<int, int> dst_coor);

        void add(std::pair<int, int> coor) {
            graph_map[coor] = InnerMapCoordinatesInfo();
        }

        bool isCoorVisited(std::pair<int, int> coor) {
            return graph_map[coor].isVisited();
        }

        bool isKnown(std::pair<int, int> coor) {
            return (graph_map.find(coor) != graph_map.end());
        }

        void visitCoor(std::pair<int, int> coor) {
            return graph_map[coor].visit();
        }

        void removeCoorDirt(std::pair<int, int> coor) {
            return graph_map[coor].removeDirt();
        }

        void addWallAtCoorDirection(std::pair<int, int> coor, Direction d) {
            return graph_map[coor].addWall(d);
        }

        bool isWallInCoorDirection(std::pair<int, int> coor, Direction d) {
            return graph_map[coor].isWallInDirection(d);
        }

        void print();
    };

    InnerMap house_map;
    std::pair<int, int> curr_coor = std::pair<int, int>(0,0);
    std::size_t max_battery = 0; // assume cleaner is fully charged on startup
    std::size_t max_steps = 0;
    std::size_t curr_steps = 0;
    bool is_finished = false;

    const WallsSensor *wall_sensor_ptr = nullptr;
    const DirtSensor *dirt_sensor_ptr = nullptr;
    const BatteryMeter *battery_meter_ptr = nullptr;

    void visitCurrentLocation();

    bool isCurrentLocationVisited() {
        return house_map.isCoorVisited(curr_coor);
    }

    bool checkCurrentLocationDirt();

    Step moveToClosestDirtyLocation();

    Step moveToClosestUnvisitedLocation();

    bool checkIfNeedToReturn(Step planned_step);

public:
    void setMaxSteps(std::size_t max_steps_num) {
        max_steps = max_steps_num;
    }
	void setWallsSensor(const WallsSensor& wall_sensor){
        wall_sensor_ptr = &wall_sensor;
    }
	void setDirtSensor(const DirtSensor& dirt_sensor) {
        dirt_sensor_ptr = &dirt_sensor;
    }
	void setBatteryMeter(const BatteryMeter& battery_meter) {
        battery_meter_ptr = &battery_meter;
    }
	Step nextStep();
};

#endif