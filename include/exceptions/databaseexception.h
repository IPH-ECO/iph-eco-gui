#ifndef DATABASEEXCEPTION_H
#define DATABASEEXCEPTION_H

#include <exception>
#include <iostream>

class DatabaseException : public std::exception {
private:
    const char* cause;
public:
    // TODO: Change constructor argument type
    DatabaseException(const char* cause);

    const char* what() const throw();
};

#endif // DATABASEEXCEPTION_H
