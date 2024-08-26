#include "algorithm.h"
#include "enums.h"
#include <queue>



void Algo_204666085::InnerMap::resetBfsStatus() {
    for (auto& [key,value] : graph_map) {
        value.setBfsStatus(false);
    }
}

// assumption : src_coor is visited and not dirty - if returned value is src_coor -> none found (that's how the algo works)
void Algo_204666085::InnerMap::bfsAlgorithm(std::pair<int, int> src_coor) { 
    closet_visited_and_dirty_flag = false;
    closet_unvisited_flag = false;

    std::queue<std::pair<int, int>> coor_queue;
    graph_map[src_coor].setBfsDistance(0);
    coor_queue.push(src_coor);
    while (!coor_queue.empty()) {

        std::pair<int, int> curr_coor = coor_queue.front();
        coor_queue.pop();

        graph_map[curr_coor].setBfsStatus(true);

        if ((!closet_visited_and_dirty_flag) && graph_map[curr_coor].isVisited() && graph_map[curr_coor].isDirty()) {
            closet_visited_and_dirty_flag = true;
            closet_visited_and_dirty = curr_coor;
        }

        if ((!closet_unvisited_flag) && (!graph_map[curr_coor].isVisited())) {
            closet_unvisited_flag = true;
            closet_unvisited = curr_coor;
        }

        if (graph_map[curr_coor].isVisited()) { // only if visited - i want to add adj to queue
            std::vector<int> numbers = {0, 1, 2, 3};
            std::shuffle(numbers.begin(), numbers.end(), gen);
            for (int i : numbers) {
                if (!graph_map[curr_coor].isWallInDirection((Direction)i) ) {
                    std::pair<int, int> next_coor = getAdvancedCoorByDirection(curr_coor, (Direction)i);
                    if (!graph_map[next_coor].isBfsSearched()) {
                        graph_map[next_coor].setBfsParent(getOppositeDirection((Direction)i));
                        graph_map[next_coor].setBfsDistance(graph_map[curr_coor].getBfsDistance() + 1);
                        coor_queue.push(next_coor);
                    }
                }
            }
        }
    }
    resetBfsStatus();
}

// assumption : both src_coor and dst_coor both are in InnerMap, and are different
Direction Algo_204666085::InnerMap::getDirection(std::pair<int, int> src_coor, std::pair<int, int> dst_coor) {
    std::pair<int, int> parent_coor;
    Direction parent_direction;
    std::pair<int, int> curr_coor = dst_coor;
    do
    {
        parent_direction = graph_map[curr_coor].getBfsParent();
        curr_coor = getAdvancedCoorByDirection(curr_coor, parent_direction);
    } while (curr_coor != src_coor);
    return getOppositeDirection(parent_direction);
}