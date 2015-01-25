#ifndef DATABASEEXCEPTION_H
#define DATABASEEXCEPTION_H

#include <exception>
#include <iostream>

class DatabaseException : public std::exception {
private:
    const char* cause;
public:
    DatabaseException(const char* cause);

    const char* what() const throw();
};

#endif // DATABASEEXCEPTION_H
