#ifndef DATABASEEXCEPTION_H
#define DATABASEEXCEPTION_H

#include <exception>

#include <QString>

class DatabaseException : public std::exception {
private:
    const char* cause;
public:
    DatabaseException(const QString &cause);

    const char* what() const throw();
};

#endif // DATABASEEXCEPTION_H
