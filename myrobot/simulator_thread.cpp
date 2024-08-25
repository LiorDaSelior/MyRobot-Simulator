#include "simulator_manager.h"
#include <boost/asio.hpp>

SimulatorManager::SimulatorThread::SimulatorThread(SimulatorManager& simulator_manager, int index_) : manager(simulator_manager), index(index_) {}

void SimulatorManager::SimulatorThread::setAlgorithmData(int job_number) {
    int current_algo_num = job_number / manager.house_vector.size();
    //std::cout << "Job algo: "<< current_algo_num << '\n';
    current_algo_name = manager.algo_name_vector[current_algo_num];
    //std::cout << "Algo pnt in vector before move: " << manager.algo_pnt_vector[job_number].get() << " - for job " << job_number << '\n';
    current_algo_pnt = std::move(manager.algo_pnt_vector[job_number]);
    //std::cout << "Algo pnt in vector after move: " << manager.algo_pnt_vector[job_number].get() << " - for job " << job_number << '\n';
    //std::cout << "Algo pnt in setAlgorithmData after move: " << current_algo_pnt << " - for job " << job_number << '\n';
}

bool SimulatorManager::SimulatorThread::runJob(int job_number) { 
    //auto& current_timer = manager.getJobTimer(job_number);
    bool is_elapsed = true;

    int current_house_num = job_number % manager.house_vector.size();

    setAlgorithmData(job_number);
    MySimulator simulator(manager.house_vector[current_house_num]);
    simulator.setAlgorithm(std::move(current_algo_pnt));

    manager.startJobTimer(job_number, index);

    simulator.run();

    manager.timerJobTimerLock(job_number);
    if (!manager.isJobTimerFinished(job_number))
        manager.finishJobTimer(job_number);
        is_elapsed = false; // got here before timer check
    manager.timerJobTimerUnlock(job_number);

    if (!is_elapsed) {
        std::cout << index  << " thread is done with job #"<< job_number << std::endl;
        if (!manager.summary_only) 
            simulator.output(current_algo_name);
        manager.setScoreboardJobScore(job_number, simulator.calculateScore());
        return true;
    }
    else {
        std::cout << index  << " thread gave up on job #"<< job_number << std::endl;
        //! print to .error
    }
    return false;
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
            check = runJob(current_job);
        }
    }
    //manager.setThreadFinished(index);
}