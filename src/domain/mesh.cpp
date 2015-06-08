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
#include <QDebug>

Mesh::Mesh() : boundaryPolygon(NULL), coordinatesDistance(0.0), showBoundaryEdges(true), showMesh(true) {}

Mesh::~Mesh() {
    this->clear();
}

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

MeshPolygon* Mesh::addMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType) {
    MeshPolygon *meshPolygon = new MeshPolygon(filename, meshPolygonType);

    try {
        meshPolygon->build();
        meshPolygon->filter(coordinatesDistance);
    } catch (MeshPolygonException &e) {
        delete meshPolygon;
        throw e;
    }

    switch (meshPolygonType) {
        case MeshPolygonType::BOUNDARY:
        delete this->boundaryPolygon;
        this->boundaryPolygon = meshPolygon;
        break;

        case MeshPolygonType::ISLAND:
        this->islands.append(meshPolygon);
        break;

        case MeshPolygonType::REFINEMENT_AREA:
        this->refinementAreas.append(meshPolygon);
        break;
    }

    return meshPolygon;
}

MeshPolygon* Mesh::getBoundaryPolygon() {
    return boundaryPolygon;
}

QList<MeshPolygon*> Mesh::getIslands() {
    return islands;
}

QList<MeshPolygon*> Mesh::getRefinementAreas() {
    return refinementAreas;
}

void Mesh::removeMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType) {
    for (QList<MeshPolygon*>::iterator it = islands.begin(); it != islands.end(); it++) {
        MeshPolygon *island = *it;

        if (island->getFilename() == filename && island->getMeshPolygonType() == meshPolygonType) {
            islands.removeOne(island);
            delete island;
            return;
        }
    }
}

MeshPolygon* Mesh::getMeshPolygon(const QString &filename, const MeshPolygonType &meshPolygonType) {
    // QList<MeshPolygon>::iterator it = qFind(domain.begin(), domain.end(), meshPolygon);

    // if (it == domain.end()) {
        return NULL;
    // }

    // return &(*it);
}

void Mesh::setShowBoundaryEdges(const bool &toggle) {
    this->showBoundaryEdges = toggle;
}

bool Mesh::getShowBoundaryEdges() const {
    return this->showBoundaryEdges;
}

void Mesh::setShowMesh(const bool &toggle) {
    this->showMesh = toggle;
}

bool Mesh::getShowMesh() const {
    return this->showMesh;
}

void Mesh::clear() {
    name.clear();
    delete boundaryPolygon;
    boundaryPolygon = NULL;

    for (QList<MeshPolygon*>::iterator it = islands.begin(); it != islands.end(); it++) {
        delete *it;
    }
    islands.clear();

    for (QList<MeshPolygon*>::iterator it = refinementAreas.begin(); it != refinementAreas.end(); it++) {
        delete *it;
    }
    refinementAreas.clear();
}

double Mesh::area() {
    double area = boundaryPolygon->area();

    for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
        area -= (*it)->area();
    }

    return area;
}
