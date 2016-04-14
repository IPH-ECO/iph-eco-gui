#ifndef MESH_EXCEPTION_H
#define MESH_EXCEPTION_H

#include <string>
#include <stdexcept>

class MeshException : public std::runtime_error {
public:
    MeshException(const std::string cause) : std::runtime_error(cause) {}
};

#endif // MESH_EXCEPTION_H
