#include "simulator.h"
#include <dlfcn.h>
#include "AlgorithmRegistrar.h"
#include <filesystem>

int main(int argc, char const *argv[])
{
    // if (argc != 2) {
    //     std::cout << "Argument amount is invalid - please enter only file directory" << std::endl;
    //     exit(1);
    // }
    // House house(argv[1]);
    

    std::string algo_path = "/home/liorubantu/Code/homework/cpp/cpp_project_3/exe/algorithms"; //TODO add path handling
    std::string house_path = "/home/liorubantu/Code/homework/cpp/cpp_project_3/exe/houses";

    std::vector<void*> handle_vector;
    std::vector<House> house_vector;

    for (const auto & algo_entry : std::filesystem::directory_iterator(algo_path)) {
        void* handle = dlopen(algo_entry.path().string().c_str(), RTLD_LAZY);
        if (!handle) {
            std::cerr << "Error loading library: " << dlerror() << std::endl;
            return 1;
        }
        handle_vector.push_back(handle);
    }

    for (const auto & house_entry : std::filesystem::directory_iterator(house_path)) {
        house_vector.emplace_back(house_entry.path().string()); //TODO add error handling
    }
    
    for (auto& house : house_vector)
    {
        for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) { // AlgorithmRegistrar acts as algorithm vector
            MySimulator simulator(house);
            std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
            simulator.setAlgorithm(std::move(algorithm));
            simulator.run();
            simulator.output(algo.name());
        }
    }
    printf("before dlclose\n");
    for (auto& handle : handle_vector)
    {
        printf("before dlclose *\n");
        dlclose(handle);
    }
}