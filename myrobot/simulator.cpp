#include "simulator.h"
#include <fstream>
#include <iostream>
#include "my_battery_meter.h"
#include "my_dirt_sensor.h"
#include "my_wall_sensor.h"

MySimulator::MySimulator(House& house) : simulator_data(house){
    filename = house.getHouseFilename();
    wall_sensor = std::make_unique<MyWallsSensor>(simulator_data);
    dirt_sensor = std::make_unique<MyDirtSensor>(simulator_data);
    battery_meter = std::make_unique<MyBatteryMeter>(simulator_data);
}


void MySimulator::setAlgorithm(std::unique_ptr<AbstractAlgorithm> abstract_algorithm_pnt){
    algorithm = std::move(abstract_algorithm_pnt);
    (*algorithm).setMaxSteps(simulator_data.getMaxMissionSteps());
    (*algorithm).setWallsSensor(*wall_sensor);
    (*algorithm).setDirtSensor(*dirt_sensor);
    (*algorithm).setBatteryMeter(*battery_meter);
}

void MySimulator::run() {
    printf("SIMULATION START\n");
    Step curr_step = static_cast<Step>(0);
    simulator_data.reset();
    while (step_count <= simulator_data.getMaxMissionSteps() && (!is_finished) && !(curr_step == Step::Finish && simulator_data.isVacuumAtDocking())) {
        curr_step = (*algorithm).nextStep();

        if (step_count == simulator_data.getMaxMissionSteps()) { // forum - there can be "max_steps + 1" if the last one is finish
            if (curr_step == Step::Finish) {
                step_list.push_back(curr_step);
                is_finished = true;
            }
            break;
        }

        step_list.push_back(curr_step);
        if (!simulator_data.applyStep(curr_step)) {
            //printf("WARNING: STEP DOES NOTHING\n");
        }
        if (curr_step != Step::Finish)
            step_count++;
        else
            is_finished = true;
    }
    printf("SIMULATION END\n");
}

void MySimulator::output(const std::string& algorithm_name) {
    std::string output_filename = ".txt";
    output_filename.insert(0, algorithm_name);
    output_filename.insert(0, "-");
    output_filename.insert(0, filename);

    std::ofstream output_file(output_filename);
    if (!output_file) {
        std::cerr << "Error opening file: " << output_filename << std::endl;
        return;
    }

    output_file << "NumSteps = " << step_count << std::endl;
    output_file << "DirtLeft = "<< simulator_data.getTotalDirt() << std::endl;
    output_file << "Status = "<< getStatus() << std::endl;
    output_file << "InDock = "<< (simulator_data.isVacuumAtDocking()? "TRUE" : "FALSE") << std::endl;
    output_file << "Score = "<< calculateScore() << std::endl;
    output_file << "Steps:" << std::endl;
    for (auto &&temp_step : step_list)
    {
        switch (temp_step)
        {
        case Step::Finish:
            output_file << 'F';
            break;
        case Step::Stay:
            output_file << 's';
            break;
        case Step::North:
            output_file << 'N';
            break;
        case Step::East:
            output_file << 'E';
            break;
        case Step::South:
            output_file << 'S';
            break;
        case Step::West:
            output_file << 'W';
            break;
        }   
    }
    output_file.flush();
    output_file.close();
}

int MySimulator::calculateScore() {
    std::string status = getStatus();
    int score = simulator_data.getTotalDirt() * 300;
    if (status == "DEAD")
        score += simulator_data.getMaxMissionSteps() + 2000;
    else if (status == "FINISHED" && !simulator_data.isVacuumAtDocking())
        score += simulator_data.getMaxMissionSteps() + 3000;
    else
        score += step_count + (simulator_data.isVacuumAtDocking() ? 0 : 1000);
    return score;
}

std::string MySimulator::getStatus() {
    std::string status;
    if (simulator_data.batteryMeterQuery() <= 0) {
        if (simulator_data.isVacuumAtDocking()) {
            if (is_finished)
                status = "FINISHED";
            else
                status = "WORKING";
        }
        else {
            status = "DEAD";
        }
    }
    else {
        if (is_finished)
            status = "FINISHED";
        else
            status = "WORKING";
    }
    return status;
}