#include "include/exceptions/simulation_exception.h"

SimulationException::SimulationException(const QString &cause) {
    int length = cause.size() * sizeof(char);
    
    this->cause = (char*) malloc(length);
    memset(this->cause, 0, length);
    strcpy(this->cause, cause.toStdString().c_str());
}

const char* SimulationException::what() const throw() {
    return cause;
}
