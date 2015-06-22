#include "include/exceptions/grid_data_exception.h"

GridDataException::GridDataException(const QString &cause) {
    int length = cause.size() * sizeof(char);
    
    this->cause = (char*) malloc(length);
    memset(this->cause, 0, length);
    strcpy(this->cause, cause.toStdString().c_str()); //TODO: test if this works on Windows
}

const char* GridDataException::what() const throw() {
    return cause;
}
