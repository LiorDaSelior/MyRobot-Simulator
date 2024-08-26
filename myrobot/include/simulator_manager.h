#ifndef SIMULATOR_MANAGER_H
#define SIMULATOR_MANAGER_H

#include "simulator.h"
#include "AlgorithmRegistrar.h"
#include <condition_variable>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <custom_exception.h>

class SimulatorManager {
public:
    SimulatorManager(int argc, char const *argv[]);
    bool run();
    void close();

private:
    class SimulatorThread {
    private:
        SimulatorManager& manager;
        std::unique_ptr<AbstractAlgorithm> current_algo_pnt;
        std::string current_algo_name;
        int index;

        void setAlgorithmData(int job_number);
        bool runJob(int job_number);

    public:
        SimulatorThread(SimulatorManager& simulator_manager, int index);
        void run();
    };

    class SimulatorJobTimer {
        SimulatorJobTimer& operator=(const SimulatorJobTimer&) = delete;
        SimulatorJobTimer(const SimulatorJobTimer&) = delete;
    private:
        int job_total_time;
        int worker = -1;
        std::chrono::time_point<std::chrono::steady_clock> start_time;
        bool is_finished = false;
        bool is_started = false;
        std::mutex timer_mutex;

        void assign(int index) { worker = index; }

    public:
        SimulatorJobTimer(){};
        SimulatorJobTimer(int total_time) : job_total_time(total_time){};
        void start(int worker_index) { 
            assign(worker_index);
            start_time = std::chrono::steady_clock::now(); 
            is_started = true;
            }
        void finish() { is_finished = true; }
        std::chrono::time_point<std::chrono::steady_clock> getStartTime() {return start_time;}
        bool isStarted() {return is_started;}
        bool isFinished() {return is_finished;}
        int getWorkerIndex() {return worker;}
        int getTotalTime() {return job_total_time;}
        void timerLock() {timer_mutex.lock();}
        void timerUnlock() {timer_mutex.unlock();}
    };

    std::string algo_path = ".";
    std::string house_path = ".";
    int threads_num = 10;
    bool summary_only = false;
    std::vector<void*> handle_vector;
    std::vector<House> house_vector;
    std::vector<std::unique_ptr<AbstractAlgorithm>> algo_pnt_vector;
    std::vector<std::string> algo_name_vector;
    int job_total;
    int job_number;
    std::mutex job_mutex;
    std::vector<int> scoreboard;
    std::vector<std::jthread> threads;
    std::vector<bool> is_thread_stuck_vector;
    std::unordered_map<int, SimulatorJobTimer> timer_dict;
    int total_time = 0;

    void handleSimulatorException(SimulatorException& e);
    void outputSimulatorError(std::string filename, std::string error_msg);
    void handleArguments(int argc, char const *argv[]);
    void setScoreboardJobScore(int job_number, int score) {
        scoreboard[job_number] = score;
    }
    SimulatorJobTimer& getJobTimer(int job_number) {
        return timer_dict[job_number];
    }
    void outputCSV();
    void manageJobs();

    void startJobTimer(int i, int worker_index) {timer_dict[i].start(worker_index);}
    void finishJobTimer(int i) { timer_dict[i].finish(); }
    std::chrono::time_point<std::chrono::steady_clock> getJobTimerStartTime(int i) {return timer_dict[i].getStartTime();}
    bool isJobTimerStarted(int i) {return timer_dict[i].isStarted();}
    bool isJobTimerFinished(int i) {return timer_dict[i].isFinished();}
    int getJobTimerWorkerIndex(int i) {return timer_dict[i].getWorkerIndex();}
    int getJobTimerTotalTime(int i) {return timer_dict[i].getTotalTime();}
    void timerJobTimerLock(int i) {timer_dict[i].timerLock();}
    void timerJobTimerUnlock(int i) {timer_dict[i].timerUnlock();}
};

#endif