#include "include/exceptions/mesh_exception.h"

MeshException::MeshException(const QString &cause) {
    int length = cause.size() * sizeof(char);
    
    this->cause = (char*) malloc(length);
    memset(this->cause, 0, length);
    strcpy(this->cause, cause.toStdString().c_str());
}

const char* MeshException::what() const throw() {
    return cause;
}
