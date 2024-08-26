#include "simulator_manager.h"
#include <fstream>
#include <filesystem>
#include <dlfcn.h>
#include <chrono>


constexpr int TIME_QUANTUM = 3; // ms

SimulatorManager::SimulatorManager(int argc, char const *argv[]) {
    handleArguments(argc, argv);

    for (const auto & dirent : std::filesystem::directory_iterator(algo_path)) {
        int current_size;
        if (dirent.path().extension() == ".so") {
            current_size = AlgorithmRegistrar::getAlgorithmRegistrar().count(); 
            try {
                void* handle = dlopen(dirent.path().c_str(), RTLD_LAZY);
                if (handle == nullptr) {
                    throw AlgorithmFailedRegistration(dirent.path().stem().string(),dirent.path().string());
                }
                else {
                    if (current_size == static_cast<int>(AlgorithmRegistrar::getAlgorithmRegistrar().count())) {
                        throw AlgorithmFailedRegistration(dirent.path().stem().string(),dirent.path().string());
                    }
                    else {
                    handle_vector.push_back(handle);  
                    }  
                }
            }
            catch (AlgorithmFailedRegistration& e) {
                handleSimulatorException(e);
            }
        }
    }

    for (const auto & dirent : std::filesystem::directory_iterator(house_path)) {
        if (dirent.path().extension() == ".house") {
            try {
            house_vector.emplace_back(dirent.path().string());
            }
            catch (SimulatorException& e) {
                handleSimulatorException(e);
            }
        }
    }
    job_total = AlgorithmRegistrar::getAlgorithmRegistrar().count() * house_vector.size();
    algo_pnt_vector = std::vector<std::unique_ptr<AbstractAlgorithm>>(job_total);
    algo_name_vector = std::vector<std::string>(AlgorithmRegistrar::getAlgorithmRegistrar().count());

    timer_dict.clear();

    int current_job_num = 0;
    int current_algo_num = 0;
    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) { // AlgorithmRegistrar acts as algorithm vector
        algo_name_vector[current_algo_num] = algo.name();
        current_algo_num++;
        for (auto& house : house_vector)
        {
            algo_pnt_vector[current_job_num] = std::move(algo.create());
            timer_dict.emplace(current_job_num, house.getMaxMissionSteps() * TIME_QUANTUM);
            current_job_num++;
        }
    }
    scoreboard = std::vector<int>(job_total);
    threads = std::vector<std::jthread>();
    is_thread_stuck_vector = std::vector<bool>(threads_num, false);
}

bool SimulatorManager::run() {
    job_number = 0;

    for (int i = 0; i < threads_num; i++)
    {
        threads.emplace_back(([this] (int index) {
            SimulatorThread sim_thread = SimulatorThread(*this, index);
            sim_thread.run();
            }), i
        );
    }

    manageJobs();

    for (size_t i = 0; i < threads.size(); i++)
    {
        if ((!is_thread_stuck_vector[i]) && threads[i].joinable()) {
            threads[i].join();
        }
    }

    outputCSV();

    for (size_t i = 0; i < threads.size(); i++)
    {
        if (is_thread_stuck_vector[i] && threads[i].joinable()) {
            threads[i].join();
        }
    }

    return true;
}

void SimulatorManager::close() {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    for (auto& handle : handle_vector) {
        dlclose(handle);
    }
    handle_vector.clear();
}

void SimulatorManager::handleArguments(int argc, char const *argv[]) {
    std::unordered_map<std::string, std::string> parameters = {
        {"-house_path", "."},
        {"-algo_path", "."},
        {"-num_threads", "10"}            
    };

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-summary_only") {
            summary_only = true;
            continue;
        }
        
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);

            if (parameters.find(key) != parameters.end()) {
                parameters[key] = value;
            } else {
                std::cerr << "Unknown argument: " << key << std::endl;
            }
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
        }
    }

    house_path = parameters["-house_path"];
    algo_path = parameters["-algo_path"];
    threads_num = std::stoi(parameters["-num_threads"]);
}

void SimulatorManager::outputCSV() {
    std::string output_filename = "summary.csv";

    std::ofstream output_file = std::ofstream(output_filename);
    if (!output_file) {
        std::cerr << "Error opening file: " << output_filename << std::endl;
        return;
    }

    output_file << "algorithms/houses";

    for (size_t i = 0; i < house_vector.size(); i++)
    {
        output_file << ",";
        output_file << house_vector[i].getHouseFilename();     
    }
    output_file << std::endl;
    for (size_t i = 0; i < algo_name_vector.size(); i++)
    {
        output_file << algo_name_vector[i];
        for (size_t j = 0; j < house_vector.size(); j++)
        {
            output_file << ",";
            output_file << scoreboard[i * house_vector.size() + j];
        }
        output_file << std::endl;
    }
    output_file.close();
}

void SimulatorManager::manageJobs() {
    bool isJobLeft = true;
    while(isJobLeft) {
        isJobLeft = false;
        int elapsed;
        bool is_elapsed;
        int current_score;
        for (int i = 0; i < job_total; i++)
        {
            is_elapsed = false;
            timer_dict[i].timerLock();
            if (!timer_dict[i].isFinished()) {
                if (timer_dict[i].isStarted()) {
                    elapsed = duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timer_dict[i].getStartTime()).count();
                    if (elapsed >= timer_dict[i].getTotalTime()) {
                        timer_dict[i].finish();
                        is_elapsed = true;
                    }
                    else
                        isJobLeft = true;
                }
                else
                    isJobLeft = true;
            }
            timer_dict[i].timerUnlock();
            if (is_elapsed) {
                is_thread_stuck_vector[timer_dict[i].getWorkerIndex()] = true; // set thread to stuck

                current_score = house_vector[i % house_vector.size()].getMaxMissionSteps() * 2 + 
                house_vector[i % house_vector.size()].getTotalDirt() * 300 + 2000;
                setScoreboardJobScore(i, current_score); // set scoreboard for failed job

                std::string error_msg = "Timeout occurred when running simulation on house \"" + house_vector[i % house_vector.size()].getHouseFilename() + "\". Recording timeout score in summary.csv";
                outputSimulatorError(algo_name_vector[static_cast<int>(i / house_vector.size())], error_msg);

                is_thread_stuck_vector.push_back(false); // add new thread
                threads.emplace_back(([this] () { 
                SimulatorThread sim_thread = SimulatorThread(*this, threads.size());
                sim_thread.run();
                }));
            }
        }
    }
}

void SimulatorManager::handleSimulatorException(SimulatorException& e) {
    outputSimulatorError(e.getFilename(), e.getMessage());
}

void SimulatorManager::outputSimulatorError(std::string filename, std::string error_msg) {
    std::string error_filename = filename + ".error";
    std::ofstream error_file = std::ofstream(error_filename, std::ios::app);
    if (!error_file) {
        std::cerr << "Error opening file: " << error_filename << std::endl;
        return;
    }
    error_file << error_msg << std::endl;
    error_file.close();
}