#include "simulator_manager.h"

SimulatorManager::SimulatorThread::SimulatorThread(SimulatorManager& simulator_manager) : manager(simulator_manager){
}

void SimulatorManager::SimulatorThread::setAlgorithmFromIndex(int index) {
    int i = 0;
        for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) { 
            if (i == index) {
                current_algo_pnt = algo.create();
                current_algo_name = algo.name();
                break;
            }
            i++;
        }
}

void SimulatorManager::SimulatorThread::runPair(int current_algo_num, int current_house_num) { 
    MySimulator simulator(manager.house_vector[current_house_num]);
    setAlgorithmFromIndex(current_algo_num);
    simulator.setAlgorithm(std::move(current_algo_pnt));
    simulator.run();
    if (!manager.summary_only) 
        simulator.output(current_algo_name);
};

void SimulatorManager::SimulatorThread::run() {
    int current_job = 0;
    bool check = true;
    while (check) {
        manager.job_mutex.lock();
        check = manager.job_number < manager.job_total;
        if (check) {
            current_job = manager.job_number;
            manager.job_number++;
            std::cout << "+\n";
        }
        manager.job_mutex.unlock();
        if (check) {
            std::cout << "Job current number: ("<< current_job << "/" << manager.job_total << ")\n";
            std::cout << "Job algo: "<< current_job / manager.house_vector.size() << '\n';
            std::cout << "Job house: "<< current_job % manager.house_vector.size() << '\n';
            runPair(current_job / manager.house_vector.size(), current_job % manager.house_vector.size());
        }
    }
}