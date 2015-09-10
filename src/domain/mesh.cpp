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
            for (MeshPolygon *islandsPolygon : islands) {
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

double Mesh::getLatitudeAverage() const {
    QList<MeshPolygon*> meshPolygons = islands + refinementAreas;
    double latitudeAverage = 0;
    
    if (boundaryPolygon != nullptr) {
        meshPolygons.prepend(boundaryPolygon);
    }
    
    for (MeshPolygon *meshPolygon : meshPolygons) {
        latitudeAverage += meshPolygon->getLatitudeAverage();
    }
    
    return latitudeAverage / (double) meshPolygons.size();
}

void Mesh::cancelGeneration(bool value) {
    this->generationCanceled = value;
}

void Mesh::removeMeshPolygon(const QString &name, const MeshPolygonType &meshPolygonType) {
    if (meshPolygonType == MeshPolygonType::BOUNDARY) {
        throw MeshException("It isn't possible to remove the boundary polygon.");
    }
    
    QList<MeshPolygon*> meshPolygons = islands + refinementAreas;
    
    for (MeshPolygon *meshPolygon : meshPolygons) {
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
    
    for (MeshPolygon *meshPolygon : meshPolygons) {
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

    for (MeshPolygon *island : islands) {
        delete island;
    }
    islands.clear();

    for (MeshPolygon *refinementArea : refinementAreas) {
        delete refinementArea;
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

QSet<vtkIdType> Mesh::getBoundaryCellIds(vtkSmartPointer<vtkIdTypeArray> edgeIds) const {
    QSet<vtkIdType> boundaryCellIds;
    
    meshPolyData->BuildLinks();
    
    for (vtkIdType edgeId = 0; edgeId < edgeIds->GetNumberOfTuples(); edgeId++) {
        vtkSmartPointer<vtkCell> edge = boundaryPolyData->GetCell(edgeIds->GetTuple1(edgeId));
        double vertexA[3], vertexB[3];
        
        edge->GetPoints()->GetPoint(0, vertexA);
        edge->GetPoints()->GetPoint(1, vertexB);
        
        vtkIdType meshPointAId = meshPolyData->FindPoint(vertexA);
        vtkIdType meshPointBId = meshPolyData->FindPoint(vertexB);
        vtkSmartPointer<vtkIdList> cellsVertexA = vtkSmartPointer<vtkIdList>::New();
        vtkSmartPointer<vtkIdList> cellsVertexB = vtkSmartPointer<vtkIdList>::New();
        
        meshPolyData->GetPointCells(meshPointAId, cellsVertexA);
        meshPolyData->GetPointCells(meshPointBId, cellsVertexB);
        
        bool isCellFound = false;
        
        for (vtkIdType i = 0; i < cellsVertexA->GetNumberOfIds() && !isCellFound; i++) {
            for (vtkIdType j = 0; j < cellsVertexB->GetNumberOfIds() && !isCellFound; j++) {
                if (cellsVertexA->GetId(i) == cellsVertexB->GetId(j)) {
                    boundaryCellIds.insert(cellsVertexA->GetId(i));
                    isCellFound = true;
                }
            }
        }
    }
    
    return boundaryCellIds;
}

void Mesh::generateBoundaryPolyData() {
    vtkSmartPointer<vtkFeatureEdges> boundaryEdges = vtkSmartPointer<vtkFeatureEdges>::New();
    boundaryEdges->SetInputData(meshPolyData);
    boundaryEdges->BoundaryEdgesOn();
    boundaryEdges->FeatureEdgesOff();
    boundaryEdges->ManifoldEdgesOff();
    boundaryEdges->NonManifoldEdgesOff();
    boundaryEdges->Update();
    
    boundaryPolyData = vtkSmartPointer<vtkPolyData>::New();
    boundaryPolyData->ShallowCopy(boundaryEdges->GetOutput());
}