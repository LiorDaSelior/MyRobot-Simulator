#include "simulator.h"
#include <fstream>
#include <iostream>
#include "my_battery_meter.h"
#include "my_dirt_sensor.h"
#include "my_wall_sensor.h"

MySimulator::MySimulator(House& house) : simulator_data(house){
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
    Step curr_step;
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
        simulator_data.applyStep(curr_step);
        if (curr_step != Step::Finish)
            step_count++;
        else
            is_finished = true;
    }
    output();
}

void MySimulator::output() {
    std::string output_file_name = file_name;
    output_file_name.insert(0, "output_");

    std::ofstream output_file(output_file_name);
    if (!output_file) {
        std::cerr << "Error opening file: " << output_file_name << std::endl;
        return;
    }

    std::string status;
    if (simulator_data.batteryMeterQuery() <= 0) {
        if (!simulator_data.isVacuumAtDocking()) {
            status = "DEAD";
        }
        else {
            if (is_finished)
                status = "FINISHED";
            else
                status = "WORKING";
        }
    }
    else {
        if (is_finished)
            status = "FINISHED";
        else
            status = "WORKING";
    }


    output_file << "NumSteps = " << step_count << std::endl;
    output_file << "DirtLeft = "<< simulator_data.getTotalDirt() << std::endl;
    output_file << "Status = "<< status << std::endl;
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