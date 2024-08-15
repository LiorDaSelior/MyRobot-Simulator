#include "algorithm.h"
#include "AlgorithmRegistration.h"

REGISTER_ALGORITHM(Algo_123456789);

Step Algo_123456789::nextStep() {
    return Step::North;
}