#include "simulator_manager.h"
#include <filesystem>
#include <dlfcn.h>
#include <thread>


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

    int current_job_num = 0;
    int current_algo_num = 0;
    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) { // AlgorithmRegistrar acts as algorithm vector
        algo_name_vector[current_algo_num] = algo.name();
        current_algo_num++;
        for (auto& house : house_vector)
        {
            algo_pnt_vector[current_job_num] = std::move(algo.create());
            current_job_num++;
        }
    }
    //std::cout << "TOTAL: "<< current_job_num << '\n';
    for (size_t j = 0; j < current_algo_num; j++)
    {
        std::cout << "Algo name "<< j << ": " << algo_name_vector[j] << '\n';
    }
    for (size_t i = 0; i < current_job_num; i++)
    {
        std::cout << "Algo pnt "<< i << ": " << algo_pnt_vector[i] << '\n';
        
    }
    
}

void SimulatorManager::run() {
    job_number = 0;
    //std::vector<SimulatorThread> simulator_jobs;
    std::vector<std::thread> threads;
    for (int i = 0; i < threads_num; i++)
    {
        //printf("%d\n", i);

        threads.emplace_back([this] () {
            SimulatorThread sim_thread = SimulatorThread(*this);
            sim_thread.run();
            });
    }
    for (std::thread& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    

    // for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) { // AlgorithmRegistrar acts as algorithm vector
    //     MySimulator simulator(house);
    //     std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
    //     simulator.setAlgorithm(std::move(algorithm));
    //     simulator.run();
    //     if (!summary_only) {
    //         simulator.output(algo.name());
    //     }
    // }

    // for (auto& house : house_vector)
    // {
    //     for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) { // AlgorithmRegistrar acts as algorithm vector
    //         MySimulator simulator(house);
    //         std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
    //         simulator.setAlgorithm(std::move(algorithm));
    //         simulator.run();
    //         if (!summary_only) {
    //             simulator.output(algo.name());
    //         }
    //     }
    // }
}

void SimulatorManager::close() {
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