#include "include/domain/mesh.h"
#include "include/exceptions/mesh_exception.h"
#include "include/exceptions/mesh_polygon_exception.h"

#include <QStringList>
#include <QMultiMap>
#include <QFile>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QJsonDocument>
#include <QJsonArray>

#include <GeographicLib/GeoCoords.hpp>

Mesh::Mesh() : boundaryPolygon(NULL), coordinatesDistance(0.0), showDomainBoundary(true), showMesh(true) {}

Mesh::Mesh(QString &_name) : boundaryPolygon(NULL), name(_name), coordinatesDistance(0.0), showDomainBoundary(true) {}

Mesh::~Mesh() {}

void Mesh::setName(const QString &name) {
    this->name = name;
}

QString Mesh::getName() const {
    return this->name;
}

void Mesh::setCoordinatesDistance(const double &coordinatesDistance) {
    this->coordinatesDistance = coordinatesDistance;
}

double Mesh::getCoordinatesDistance() const {
    return this->coordinatesDistance;
}

void Mesh::buildDomain(const QString &filename) {
    addMeshPolygon(filename, MeshPolygonType::BOUNDARY);

    // QList<MeshPolygon*> islands = this->getIslands();

    // for (QList<MeshPolygon*>::iterator it = islands.begin(); it != islands.end(); it++) {
    //     buildMeshPolygon(*it);
    // }
}

MeshPolygon* Mesh::addMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType) {
    MeshPolygon *meshPolygon = new MeshPolygon(filename, meshPolygonType);

    try {
        meshPolygon->build();
        meshPolygon->filter(coordinatesDistance);
    } catch (MeshPolygonException &e) {
        delete meshPolygon;
        throw e;
    }

    switch (meshPolygon->getMeshPolygonType()) {
        case MeshPolygonType::BOUNDARY:
        delete boundaryPolygon;
        boundaryPolygon = meshPolygon;
        break;

        case MeshPolygonType::ISLAND:
        islands.append(meshPolygon);
        break;

        case MeshPolygonType::REFINEMENT_AREA:
        refinementAreas.append(meshPolygon);
        break;
    }

    return meshPolygon;
}

QList<MeshPolygon*> Mesh::getIslands() {
    return islands;
}

// Refactor
void Mesh::removeMeshPolygon(const MeshPolygon &meshPolygon) {
    // domain.removeOne(meshPolygon);
}

MeshPolygon* Mesh::getBoundaryPolygon() {
    return boundaryPolygon;
}

MeshPolygon* Mesh::getMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType) {
    // QList<MeshPolygon>::iterator it = qFind(domain.begin(), domain.end(), meshPolygon);

    // if (it == domain.end()) {
        return NULL;
    // }

    // return &(*it);
}

void Mesh::setShowDomainBoundary(const bool &show) {
    this->showDomainBoundary = show;
}

bool Mesh::getShowDomainBoundary() const {
    return this->showDomainBoundary;
}

void Mesh::setShowMesh(const bool &show) {
    this->showMesh = show;
}

bool Mesh::getShowMesh() const {
    return this->showMesh;
}

// TODO: Refactor
void Mesh::clear() {
    name.clear();
    // domain.clear();
    showDomainBoundary = showMesh = true;
    showUTMCoordinates = showVertexesLabels = showTrianglesLabels = showEdgesLabels = false;
}

double Mesh::area() {
    double area = boundaryPolygon->area();

    for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
        area -= (*it)->area();
    }

    return area;
}
