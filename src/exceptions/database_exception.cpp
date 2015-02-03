#include "include/exceptions/database_exception.h"

DatabaseException::DatabaseException(const QString &cause) {
    this->cause = cause.toStdString().c_str();
}

const char* DatabaseException::what() const throw() {
    return cause;
}
