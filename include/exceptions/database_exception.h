#ifndef DATABASE_EXCEPTION_H
#define DATABASE_EXCEPTION_H

#include <exception>
#include <QString>

class DatabaseException : public std::exception {
private:
    char* cause;
public:
    DatabaseException(const QString &cause);

    const char* what() const throw();
};

#endif // DATABASE_EXCEPTION_H
