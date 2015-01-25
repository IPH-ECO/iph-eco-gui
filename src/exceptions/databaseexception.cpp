#include "include/exceptions/databaseexception.h"

DatabaseException::DatabaseException(const char* cause) {
    this->cause = cause;
}

const char* DatabaseException::what() const throw() {
    return cause;
}
