#ifndef GRID_DATA_EXCEPTION_H
#define GRID_DATA_EXCEPTION_H

#include <exception>
#include <QString>

class GridDataException : public std::exception {
private:
    const char* cause;
public:
    GridDataException(const QString &cause);

    const char* what() const throw();
};

#endif // GRID_DATA_EXCEPTION_H
