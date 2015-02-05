#include "include/exceptions/mesh_exception.h"

MeshException::MeshException(const QString &cause) {
    this->cause = cause.toStdString().c_str();
}

const char* MeshException::what() const throw() {
    return cause;
}
