#include "simulator_manager.h"
#include <dlfcn.h>
#include "AlgorithmRegistrar.h"
#include <filesystem>

int main(int argc, char const *argv[])
{
    SimulatorManager sim_manager = SimulatorManager(argc, argv);
    sim_manager.run();
    //sim_manager.close();
}
