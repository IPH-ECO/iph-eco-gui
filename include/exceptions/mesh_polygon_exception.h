#ifndef MESH_POLYGON_EXCEPTION_H
#define MESH_POLYGON_EXCEPTION_H

#include <string>
#include <stdexcept>

class MeshPolygonException : public std::runtime_error {
public:
    MeshPolygonException(const std::string cause) : std::runtime_error(cause) {}
};

#endif // MESH_POLYGON_EXCEPTION_H
