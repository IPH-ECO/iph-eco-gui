#include "include/exceptions/grid_data_exception.h"

GridDataException::GridDataException(const QString &cause) {
    this->cause = cause.toStdString().c_str();
}

const char* GridDataException::what() const throw() {
    return cause;
}
