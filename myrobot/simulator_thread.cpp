#include "simulator_manager.h"

SimulatorManager::SimulatorThread::SimulatorThread(SimulatorManager& simulator_manager) : manager(simulator_manager){
}

void SimulatorManager::SimulatorThread::setAlgorithmData(int job_number) {
    int current_algo_num = job_number / manager.house_vector.size();
    //std::cout << "Job algo: "<< current_algo_num << '\n';
    current_algo_name = manager.algo_name_vector[current_algo_num];
    //std::cout << "Algo pnt in vector before move: " << manager.algo_pnt_vector[job_number].get() << " - for job " << job_number << '\n';
    current_algo_pnt = std::move(manager.algo_pnt_vector[job_number]);
    //std::cout << "Algo pnt in vector after move: " << manager.algo_pnt_vector[job_number].get() << " - for job " << job_number << '\n';
    //std::cout << "Algo pnt in setAlgorithmData after move: " << current_algo_pnt << " - for job " << job_number << '\n';
}

void SimulatorManager::SimulatorThread::runJob(int job_number) { 
    int current_house_num = job_number % manager.house_vector.size();
    //std::cout << "Job current number: ("<< job_number << "/" << manager.job_total << ")\n";
    //std::cout << "Job house: "<< current_house_num << '\n';
    setAlgorithmData(job_number);
    MySimulator simulator(manager.house_vector[current_house_num]);
    //std::cout << "Algo pnt in runJob: " << current_algo_pnt << " - for job " << job_number << '\n';
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
            //std::cout << "+\n";
        }
        manager.job_mutex.unlock();
        if (check) {
            runJob(current_job);
        }
    }
}