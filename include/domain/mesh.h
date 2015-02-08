#ifndef MESH_H
#define MESH_H

#include <QString>

class Mesh {
protected:
    QString name;
    QString boundaryFilePath;

public:
    Mesh(QString &_name);
    Mesh(QString &_name, QString &_boundaryFilePath);

    inline bool operator==(const Mesh &mesh) {
        return this->name == mesh.getName();
    }

    void setName(const QString &name);
    QString getName() const;

    void setBoundaryFilePath(const QString &boundaryFilePath);
    QString getBoundaryFilePath() const;

};

#endif // MESH_H

