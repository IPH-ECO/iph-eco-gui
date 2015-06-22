#include "include/exceptions/mesh_polygon_exception.h"

MeshPolygonException::MeshPolygonException(const QString &cause) {
    int length = cause.length() * sizeof(char);
    
    this->cause = (char*) malloc(length);
    memset(this->cause, 0, length);
    strcpy(this->cause, cause.toStdString().c_str());
}

const char* MeshPolygonException::what() const throw() {
    return cause;
}
