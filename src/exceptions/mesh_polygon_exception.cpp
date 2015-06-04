#include "include/exceptions/mesh_polygon_exception.h"

MeshPolygonException::MeshPolygonException(const QString &cause) {
    this->cause = cause.toStdString().c_str();
}

const char* MeshPolygonException::what() const throw() {
    return cause;
}
