#include "include/domain/mesh.h"
#include "include/exceptions/mesh_exception.h"
#include "include/exceptions/mesh_polygon_exception.h"

#include <QFile>
#include <QMultiMap>
#include <QIODevice>
#include <QJsonArray>
#include <QStringList>
#include <vtkCellData.h>
#include <QJsonDocument>
#include <QXmlStreamReader>
#include <vtkFeatureEdges.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkXMLPolyDataReader.h>
#include <GeographicLib/GeoCoords.hpp>

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

vtkSmartPointer<vtkPolyData> Mesh::getMeshPolyData() const {
    return meshPolyData;
}

vtkSmartPointer<vtkPolyData> Mesh::getBoundaryPolyData() const {
    return boundaryPolyData;
}

void Mesh::loadMeshPolyDataFromString(const QString &polyDataStr) {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    
    reader->SetInputString(polyDataStr.toStdString());
    reader->ReadFromInputStringOn();
    reader->Update();
    
    meshPolyData = vtkSmartPointer<vtkPolyData>::New();
    meshPolyData->DeepCopy(reader->GetOutput());
}

void Mesh::loadBoundaryPolyDataFromString(const QString &polyDataStr) {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    
    reader->SetInputString(polyDataStr.toStdString());
    reader->ReadFromInputStringOn();
    reader->Update();
    
    boundaryPolyData = vtkSmartPointer<vtkPolyData>::New();
    boundaryPolyData->DeepCopy(reader->GetOutput());
}

QString Mesh::getMeshPolyDataAsString() const {
    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    
    writer->SetFileName("meshPolyData");
    writer->SetInputData(meshPolyData);
    writer->WriteToOutputStringOn();
    writer->Write();
    
    return QString::fromStdString(writer->GetOutputString());
}

QString Mesh::getBoundaryPolyDataAsString() const {
    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    
    writer->SetFileName("meshPolyData");
    writer->SetInputData(boundaryPolyData);
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
    return meshPolyData->GetCellData()->HasArray(stdArrayName.c_str());
}

void Mesh::removeArray(const QString &arrayName) {
    std::string stdArrayName(arrayName.toStdString());
    meshPolyData->GetCellData()->RemoveArray(stdArrayName.c_str());
}

void Mesh::generateBoundaryPolyData() {
    vtkSmartPointer<vtkFeatureEdges> boundaryEdges = vtkSmartPointer<vtkFeatureEdges>::New();
    boundaryEdges->SetInputData(meshPolyData);
//    boundaryEdges->ColoringOff();
    boundaryEdges->BoundaryEdgesOn();
    boundaryEdges->FeatureEdgesOff();
    boundaryEdges->ManifoldEdgesOff();
    boundaryEdges->NonManifoldEdgesOff();
    boundaryEdges->Update();
    
    boundaryPolyData = vtkSmartPointer<vtkPolyData>::New();
    boundaryPolyData->ShallowCopy(boundaryEdges->GetOutput());
}