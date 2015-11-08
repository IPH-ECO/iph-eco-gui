#ifndef SIMULATION_EXCEPTION_H
#define SIMULATION_EXCEPTION_H

#include <exception>
#include <QString>

class SimulationException : public std::exception {
private:
    char* cause;
public:
    SimulationException(const QString &cause);

    const char* what() const throw();
};

#endif // SIMULATION_EXCEPTION_H
