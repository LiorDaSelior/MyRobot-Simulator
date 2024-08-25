#include "simulator_manager.h"
#include <fstream>
#include <filesystem>
#include <dlfcn.h>
#include <chrono>

constexpr int TIME_QUANTUM = 1; // ms

SimulatorManager::SimulatorManager(int argc, char const *argv[]) {
    handleArguments(argc, argv);

    for (const auto & dirent : std::filesystem::directory_iterator(algo_path)) {
        if (dirent.path().extension() == ".so") {
            void* handle = dlopen(dirent.path().c_str(), RTLD_LAZY);
            if (handle == nullptr) {
                std::cerr << "Error loading library: " << dlerror() << std::endl;
            }
            else
                handle_vector.push_back(handle);
        }
    }

    for (const auto & dirent : std::filesystem::directory_iterator(house_path)) {
        if (dirent.path().extension() == ".house") {
            house_vector.emplace_back(dirent.path().string()); //TODO add error handling
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

void SimulatorManager::run() {
    job_number = 0;

    for (int i = 0; i < threads_num; i++)
    {
        threads.emplace_back(([this, i] () {
            SimulatorThread sim_thread = SimulatorThread(*this, i);
            sim_thread.run();
            })
        );
    }
    
    manageJobs();

    for (int i = 0; i < is_thread_stuck_vector.size(); i++) {
        std::cout << i  << ": "<< is_thread_stuck_vector[i] << std::endl;
    }

    for (int i = 0; i < threads.size(); i++)
    {
        if ((!is_thread_stuck_vector[i]) && threads[i].joinable()) {
            threads[i].join();
        }
    }

    outputCSV();
}

void SimulatorManager::close() {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    for (auto& handle : handle_vector) {
        dlclose(handle);
    }
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
    threads_num = std::stoi(parameters["-num_threads"]); //TODO error handle
}

void SimulatorManager::outputCSV() {
    std::string output_filename = "summary.csv";

    std::ofstream output_file = std::ofstream(output_filename);
    if (!output_file) {
        std::cerr << "Error opening file: " << output_filename << std::endl;
        return;
    }

    output_file << "algorithms/houses";

    for (int i = 0; i < house_vector.size(); i++)
    {
        output_file << ",";
        output_file << house_vector[i].getHouseFilename();     
    }
    output_file << std::endl;
    for (int i = 0; i < algo_name_vector.size(); i++)
    {
        output_file << algo_name_vector[i];
        for (int j = 0; j < house_vector.size(); j++)
        {
            output_file << ",";
            output_file << scoreboard[i * house_vector.size() + j];
        }
        output_file << std::endl;
    }
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
                printf("elapsed!\n");
                printf("elapsed on job %d\n", i);
                printf("thread #%d\n", timer_dict[i].getWorkerIndex());
                is_thread_stuck_vector[timer_dict[i].getWorkerIndex()] = true; // set thread to stuck

                current_score = house_vector[job_number % house_vector.size()].getMaxMissionSteps() * 2 + 
                house_vector[job_number % house_vector.size()].getTotalDirt() * 300 + 2000;
                setScoreboardJobScore(i, current_score); // set scoreboard for failed job

                is_thread_stuck_vector.push_back(false); // add new thread
                threads.emplace_back(([this] () { 
                SimulatorThread sim_thread = SimulatorThread(*this, threads.size());
                sim_thread.run();
                }));
            }
            //(!timer_dict[i].isFinished()) ? (std::cout << i << std::endl) : (std::cout);
        }
    }
}