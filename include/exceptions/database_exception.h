#ifndef DATABASE_EXCEPTION_H
#define DATABASE_EXCEPTION_H

#include <string>
#include <stdexcept>

class DatabaseException : public std::runtime_error {
public:
    DatabaseException(const std::string cause) : std::runtime_error(cause) {};
};

#endif // DATABASE_EXCEPTION_H
