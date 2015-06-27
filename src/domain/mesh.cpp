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
#include <vtkCellData.h>

Mesh::Mesh() : id(0), boundaryPolygon(nullptr), coordinatesDistance(0.0), generationCanceled(false) {}

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

MeshPolygon* Mesh::addMeshPolygon(const QString &name, const QString &filename, const MeshPolygonType &meshPolygonType) {
    MeshPolygon *meshPolygon = getMeshPolygon(name, meshPolygonType);
    
    if (meshPolygon == nullptr || !meshPolygon->isPersisted()) {
        meshPolygon = new MeshPolygon(name, filename, meshPolygonType);
    }

    try {
        meshPolygon->build();
        meshPolygon->filter(coordinatesDistance);
        
        if (meshPolygonType == MeshPolygonType::BOUNDARY) {
            for (QList<MeshPolygon*>::const_iterator it = islands.begin(); it != islands.end(); it++) {
                MeshPolygon *islandsPolygon = *it;
                islandsPolygon->build();
                islandsPolygon->filter(coordinatesDistance);
            }
        }
    } catch (const MeshPolygonException &e) {
        delete meshPolygon;
        throw e;
    }
    
    if (!meshPolygon->isPersisted() && instanceOf("UnstructuredMesh")) {
        meshPolygon->setInitialCriteria();
    }

    addMeshPolygon(meshPolygon);

    return meshPolygon;
}

void Mesh::addMeshPolygon(MeshPolygon *meshPolygon) {
    switch (meshPolygon->getMeshPolygonType()) {
        case MeshPolygonType::BOUNDARY:
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
    writer->Write();
    
    return QString::fromStdString(writer->GetOutputString());
}

void Mesh::cancelGeneration(bool value) {
    this->generationCanceled = value;
}

void Mesh::removeMeshPolygon(const QString &name, const MeshPolygonType &meshPolygonType) {
    if (meshPolygonType == MeshPolygonType::BOUNDARY) {
        throw MeshException("It isn't possible to remove the boundary polygon.");
    }
    
    QList<MeshPolygon*> meshPolygons = islands + refinementAreas;
    
    for (QList<MeshPolygon*>::const_iterator it = meshPolygons.begin(); it != meshPolygons.end(); it++) {
        MeshPolygon *meshPolygon = *it;

        if (meshPolygon->getName() == name && meshPolygon->getMeshPolygonType() == meshPolygonType) {
            if (meshPolygonType == MeshPolygonType::ISLAND) {
                islands.removeOne(meshPolygon);
            } else {
                refinementAreas.removeOne(meshPolygon);
            }
            delete meshPolygon;
            return;
        }
    }
}

MeshPolygon* Mesh::getMeshPolygon(const QString &name, const MeshPolygonType &meshPolygonType) const {
    QList<MeshPolygon*> meshPolygons = islands + refinementAreas;
    
    if (boundaryPolygon != nullptr) {
        meshPolygons.prepend(boundaryPolygon);
    }
    
    for (QList<MeshPolygon*>::const_iterator it = meshPolygons.begin(); it != meshPolygons.end(); it++) {
        MeshPolygon *meshPolygon = *it;
        if (meshPolygon->getName() == name && meshPolygon->getMeshPolygonType() == meshPolygonType) {
            return meshPolygon;
        }
    }
    
    return nullptr;
}

void Mesh::clear() {
    name.clear();
    delete boundaryPolygon;
    boundaryPolygon = nullptr;

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

bool Mesh::hasArray(const QString &arrayName) {
    std::string stdArrayName(arrayName.toStdString());
    return polyData->GetCellData()->HasArray(stdArrayName.c_str());
}