#include "algorithm.h"
#include "AlgorithmRegistration.h"

REGISTER_ALGORITHM(AnotherAlgorithm);

Direction getOppositeDirection(Direction dir) {
    switch (dir)
    {
    case Direction::North:
        return Direction::South;
    case Direction::East:
        return Direction::West;
    case Direction::South:
        return Direction::North;
    case Direction::West:
        return Direction::East;
    default:
        return Direction::North;
    }
}

std::pair<int, int> getAdvancedCoorByDirection(std::pair<int, int> coor, Direction dir) {
    int i = 0;
    int j = 0;
    switch (dir)
    {
    case Direction::North:
        j = 1;
        break;
    case Direction::East:
        i = 1;
        break;
    case Direction::South:
        j = -1;
        break;
    case Direction::West:
        i = -1;
        break;
    }
    return std::pair<int, int>(coor.first + i, coor.second + j);
}

void AnotherAlgorithm::visitCurrentLocation() {
    if (isCurrentLocationVisited())
        return;

    house_map.visitCoor(curr_coor);
    if ((dirt_sensor_ptr -> dirtLevel()) <= 0) {
        house_map.removeCoorDirt(curr_coor);
    }
    for (int i = 0; i < 4; i++)
    {
        Direction curr_dir = (Direction)i;
        if(wall_sensor_ptr ->isWall(curr_dir))
            house_map.addWallAtCoorDirection(curr_coor, curr_dir);
        else {
            std::pair<int, int> temp_coor = getAdvancedCoorByDirection(curr_coor, curr_dir);
            if ((!house_map.isKnown(temp_coor)))
                house_map.add(temp_coor);
        }
    }
}

bool AnotherAlgorithm::checkCurrentLocationDirt() {
    if ((dirt_sensor_ptr -> dirtLevel()) > 0)
        return true;
    house_map.removeCoorDirt(curr_coor);
    return false;
}

Step AnotherAlgorithm::moveToClosestDirtyLocation() {
    std::pair<int, int> dst_coor;
    dst_coor = house_map.getClosestDirty(curr_coor);
    if (dst_coor != curr_coor)
        return (Step)house_map.getDirection(curr_coor, dst_coor);
    return Step::Stay;
}

Step AnotherAlgorithm::moveToClosestUnvisitedLocation() {
    std::pair<int, int> dst_coor;
    dst_coor = house_map.getClosestUnvisited(curr_coor);
    if (dst_coor != curr_coor)
        return (Step)house_map.getDirection(curr_coor, dst_coor);
    return Step::Stay;
}

bool AnotherAlgorithm::checkIfNeedToReturn(Step planned_step) {
    if (planned_step == Step::Finish) {
        return true;
    }
    size_t current_battery = battery_meter_ptr->getBatteryState();
    size_t distance_to_docking_station = house_map.getDistance(std::pair<int, int>(0,0));
    if (distance_to_docking_station == max_steps - curr_steps || distance_to_docking_station == ( (current_battery == 0) ? 0 : current_battery - 1 ))
        return true;
    if (planned_step != Step::Stay) {
        if (distance_to_docking_station == ( (current_battery <= 1) ? 0 : current_battery - 2 ))
            return true;
        if (distance_to_docking_station + 1 == max_steps - curr_steps)
            return true;
    }
    return false;
}

Step AnotherAlgorithm::nextStep() {
    Step next_step;
    Step temp_step;
    bool decided = false;

    if (curr_steps == 0) { // start up phase
        max_battery = battery_meter_ptr->getBatteryState();
    }
    visitCurrentLocation(); // enter unvisited locating and its neighbors into the data structure

    if (!(is_finished) && curr_coor == std::pair<int, int>(0,0) && max_battery > battery_meter_ptr->getBatteryState() && curr_steps + 1 < max_steps) { //check if need to charge - before BFS
        curr_steps++;
        return Step::Stay;
    }

    house_map.bfs(curr_coor);

    if (checkCurrentLocationDirt()) { // check if tile needs cleaning
        next_step = Step::Stay;
        decided = true;
    }

    if (!decided && (temp_step = moveToClosestDirtyLocation()) != Step::Stay) { // check closest visited dirty tile
        next_step = temp_step;
        decided = true;
    }

    if (!decided && (temp_step = moveToClosestUnvisitedLocation()) != Step::Stay) { // check closest unvisited tile instead
        next_step = temp_step;
        decided = true;
    }

    if (!decided) {// no dirty or unvisited tiles detected, therefore finishing operation
        next_step = Step::Finish;
        is_finished = true;
    }

    if (curr_coor == std::pair<int, int>(0,0) && next_step == Step::Finish) // if finished and already on dock
        return Step::Finish;

    if (checkIfNeedToReturn(next_step)) { // depending on planned step, we need to evaluate if we have enough steps to return to dock with battery = 1 or before steps limit
        if (std::pair<int, int>(0,0) == curr_coor) {
            if (curr_steps + 1 >= max_steps)
                next_step = Step::Finish;
            else
                next_step = Step::Stay;
        }
        else
            next_step = (Step)house_map.getDirection(curr_coor, std::pair<int, int>(0,0));
    }

    if (next_step != Step::Finish && next_step != Step::Stay) // depending on planned step, advance current coordinates
        curr_coor = getAdvancedCoorByDirection(curr_coor, (Direction)next_step); 

    curr_steps++;
    return next_step;
}