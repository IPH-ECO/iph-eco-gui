#include "include/exceptions/grid_exception.h"

GridException::GridException(const QString &cause) {
    this->cause = cause.toStdString().c_str();
}

const char* GridException::what() const throw() {
    return cause;
}
