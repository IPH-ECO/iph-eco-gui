#ifndef MESH_POLYGON_EXCEPTION_H
#define MESH_POLYGON_EXCEPTION_H

#include <exception>
#include <QString>

class MeshPolygonException : public std::exception {
private:
    char* cause;
public:
    MeshPolygonException(const QString &cause);
    
    const char* what() const throw();
};

#endif // MESH_POLYGON_EXCEPTION_H
