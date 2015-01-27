#ifndef GRIDEXCEPTION_H
#define GRIDEXCEPTION_H

#include <exception>
#include <QString>

class GridException : public std::exception {
private:
    const char* cause;
public:
    GridException(const QString &cause);

    const char* what() const throw();
};

#endif // GRIDEXCEPTION_H
