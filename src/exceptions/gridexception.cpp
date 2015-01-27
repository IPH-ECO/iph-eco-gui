#include "include/exceptions/gridexception.h"

GridException::GridException(const QString &cause) {
    this->cause = cause.toStdString().c_str();
}

const char* GridException::what() const throw() {
    return cause;
}
