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

        void setAlgorithmData(int job_number);
        void runJob(int job_number);
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
    std::vector<std::unique_ptr<AbstractAlgorithm>> algo_pnt_vector;
    std::vector<std::string> algo_name_vector;
    int job_total;
    int job_number;
    std::mutex job_mutex;

    //access_lock

    void handleArguments(int argc, char const *argv[]);

};

#endif