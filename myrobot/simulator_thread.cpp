#include "simulator_manager.h"
#include <boost/asio.hpp>

SimulatorManager::SimulatorThread::SimulatorThread(SimulatorManager& simulator_manager, int index_) : manager(simulator_manager), index(index_) {}

void SimulatorManager::SimulatorThread::setAlgorithmData(int job_number) {
    int current_algo_num = job_number / manager.house_vector.size();
    current_algo_name = manager.algo_name_vector[current_algo_num];
    current_algo_pnt = std::move(manager.algo_pnt_vector[job_number]);
}

bool SimulatorManager::SimulatorThread::runJob(int job_number) { 
    bool is_elapsed = true;

    int current_house_num = job_number % manager.house_vector.size();

    setAlgorithmData(job_number);
    MySimulator simulator(manager.house_vector[current_house_num]);
    simulator.setAlgorithm(std::move(current_algo_pnt));

    manager.startJobTimer(job_number, index);

    simulator.run();

    manager.timerJobTimerLock(job_number);
    if (!manager.isJobTimerFinished(job_number)) {
        manager.finishJobTimer(job_number);
        is_elapsed = false; // got here before timer check
    }
    manager.timerJobTimerUnlock(job_number);

    if (!is_elapsed) {
        if (!manager.summary_only) 
            simulator.output(current_algo_name);
        manager.setScoreboardJobScore(job_number, simulator.calculateScore());
        return true;
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
        }
        manager.job_mutex.unlock();
        if (check) {
            check = runJob(current_job);
        }
    }
}