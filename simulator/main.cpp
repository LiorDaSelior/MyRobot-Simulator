#include "simulator.h"
#include <dlfcn.h>
#include "AlgorithmRegistrar.h"

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        std::cout << "Argument amount is invalid - please enter only file directory" << std::endl;
        exit(1);
    }
    House house(argv[1]);
    MySimulator simulator(house);

    void* handle = dlopen("/home/liorubantu/Code/homework/cpp/cpp_project_3/exe/algorithms/libalgorithm_bfs.so", RTLD_LAZY);
    if (!handle) {
        std::cerr << "Error loading library: " << dlerror() << std::endl;
        return 1;
    }
    
    for(const auto& algo: AlgorithmRegistrar::getAlgorithmRegistrar()) {
        std::unique_ptr<AbstractAlgorithm> algorithm = algo.create();
        //std::cout << algo.name() << ": " << static_cast<int>(algorithm->nextStep()) << std::endl;
        simulator.setAlgorithm(std::move(algorithm));
        simulator.run();
    }
    dlclose(handle);
}