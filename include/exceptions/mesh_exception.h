#ifndef MESH_EXCEPTION_H
#define MESH_EXCEPTION_H

#include <exception>
#include <QString>

class MeshException : public std::exception {
private:
    char* cause;
public:
    MeshException(const QString &cause);

    const char* what() const throw();
};

#endif // MESH_EXCEPTION_H
