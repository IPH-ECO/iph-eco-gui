#ifndef SIMULATION_EXCEPTION_H
#define SIMULATION_EXCEPTION_H

#include <string>
#include <stdexcept>

class SimulationException : public std::runtime_error {
public:
    SimulationException(const std::string cause) : std::runtime_error(cause) {}
};

#endif // SIMULATION_EXCEPTION_H
