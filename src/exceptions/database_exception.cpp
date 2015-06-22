#include "include/exceptions/database_exception.h"

DatabaseException::DatabaseException(const QString &cause) {
    int length = cause.size() * sizeof(char);
    
    this->cause = (char*) malloc(length);
    memset(this->cause, 0, length);
    strcpy(this->cause, cause.toStdString().c_str());
}

const char* DatabaseException::what() const throw() {
    return cause;
}
