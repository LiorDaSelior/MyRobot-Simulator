#ifndef SIMULATOR_MANAGER_H
#define SIMULATOR_MANAGER_H

#include "simulator.h"
#include "AlgorithmRegistrar.h"

class SimulatorManager {
public:
    SimulatorManager(int argc, char const *argv[]);
    void run();
    void close();

private:
    class SimulatorThread {
    private:
        SimulatorManager& manager;
        std::unique_ptr<AbstractAlgorithm> current_algo_pnt;
        std::string current_algo_name;

        void setAlgorithmFromIndex(int index);
        void runPair(int current_algo_num, int current_house_num);
    public:
        SimulatorThread(SimulatorManager& simulator_manager);
        void run();
    };

    std::string algo_path = ".";
    std::string house_path = ".";
    int threads_num = 10;
    bool summary_only = false;
    std::vector<void*> handle_vector;
    std::vector<House> house_vector;
    std::vector<std::vector<size_t>> scoreboard;
    int job_total;
    int job_number;
    std::mutex job_mutex;

    //access_lock

    void handleArguments(int argc, char const *argv[]);
};

#endif