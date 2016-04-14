#ifndef GRID_DATA_EXCEPTION_H
#define GRID_DATA_EXCEPTION_H

#include <string>
#include <stdexcept>

class GridDataException : public std::runtime_error {
public:
    GridDataException(const std::string cause) : std::runtime_error(cause) {}
};

#endif // GRID_DATA_EXCEPTION_H
