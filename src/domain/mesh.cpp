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
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>

Mesh::Mesh() : id(0), boundaryPolygon(NULL), coordinatesDistance(0.0), generationCanceled(false), showBoundaryEdges(true), showMesh(true) {}

Mesh::~Mesh() {
    this->clear();
}

void Mesh::setId(const uint &id) {
    if (!isPersisted()) {
        this->id = id;
    }
}

uint Mesh::getId() const {
    return id;
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
    
    if (instanceOf("UnstructuredMesh")) {
        meshPolygon->setInitialCriteria();
    }

    addMeshPolygon(meshPolygon);

    return meshPolygon;
}

void Mesh::addMeshPolygon(MeshPolygon *meshPolygon) {
    switch (meshPolygon->getMeshPolygonType()) {
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

vtkPolyData* Mesh::getPolyData() {
    return polyData;
}

void Mesh::loadMeshPolygonsFromStringPolyData(const QString &polyDataStr) {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    
    reader->SetInputString(polyDataStr.toStdString());
    reader->ReadFromInputStringOn();
    reader->Update();
    
    polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->DeepCopy(reader->GetOutput());
}

QString Mesh::getPolyDataAsString() const {
    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    
    writer->SetFileName("MeshPolyData");
    writer->SetInputData(this->polyData);
    writer->WriteToOutputStringOn();
    writer->Update();
    writer->Write();
    
    return QString::fromStdString(writer->GetOutputString());
}

void Mesh::cancelGeneration(bool value) {
    this->generationCanceled = value;
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

// Refactor
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

bool Mesh::isPersisted() const {
    return this->id != 0;
}