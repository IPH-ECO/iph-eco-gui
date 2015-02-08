#include "include/domain/mesh_manager.h"

MeshManager* MeshManager::instance = NULL;

MeshManager::MeshManager() : structuredMeshes(QList<StructuredMesh>()), unstructuredMeshes(QList<UnstructuredMesh>()) {}

MeshManager* MeshManager::getInstance() {
    if (instance == NULL) {
        instance = new MeshManager();
    }

    return instance;
}

void MeshManager::addMesh(MeshType meshType, QString &meshName) {
    if (meshType == STRUCTURED) {
        this->structuredMeshes.push_back(StructuredMesh(meshName));
    } else {
        this->unstructuredMeshes.push_back(UnstructuredMesh(meshName));
    }
}

void MeshManager::removeMesh(MeshType meshType, QString &meshName) {
    if (meshType == STRUCTURED) {
        this->structuredMeshes.removeOne(StructuredMesh(meshName));
    } else {
        this->unstructuredMeshes.removeOne(UnstructuredMesh(meshName));
    }
}

bool MeshManager::meshExists(MeshType meshType, QString &meshName) {
    if (meshType == STRUCTURED) {
        return this->structuredMeshes.contains(StructuredMesh(meshName));
    }

    return this->unstructuredMeshes.contains(UnstructuredMesh(meshName));
}

Mesh* MeshManager::getMesh(MeshType meshType, QString &meshName) {
    if (meshType == STRUCTURED) {
        return &(*std::find(this->structuredMeshes.begin(), this->structuredMeshes.end(), StructuredMesh(meshName)));
    }

    return &(*std::find(this->unstructuredMeshes.begin(), this->unstructuredMeshes.end(), UnstructuredMesh(meshName)));
}

QJsonObject MeshManager::getBoundaryJson(Mesh *mesh) {
    QFile boundaryFile(mesh->getBoundaryFilePath());

    if (!boundaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw MeshException(QString("Unable to open boundary file. Error: %1").arg(boundaryFile.errorString()));
    }

    QXmlStreamReader kml(&boundaryFile);
    QJsonArray outerBoundaryJsonArray, innerBoundaryJsonArray;

    this->setObservableBoundaryFileSize(boundaryFile.size());

    while (!kml.atEnd()) {
        if (this->isCurrentTaskCanceled()) {
            break;
        }

        this->setObservableCurrentBoundaryFileSize(boundaryFile.pos());

        kml.readNext();

        if (((kml.name() == "outerBoundaryIs"  && outerBoundaryJsonArray.isEmpty()) || kml.name() == "innerBoundaryIs") && kml.isStartElement()) {
            QString boundaryElementName = kml.name().toString();

            do {
                kml.readNext();
                this->setObservableCurrentBoundaryFileSize(boundaryFile.pos());
            } while (kml.name() != "coordinates" && !kml.atEnd());

            if (kml.atEnd()) {
                throw MeshException(QString("No boundary domain found."));
            }

            // Convert geographic coordinates to UTM coordinates
            QString coordinatesText = kml.readElementText();
            QJsonArray coordinatesJsonArray;
            QStringList coordinates = coordinatesText.trimmed().split(" ");

            for (int i = 0; i < coordinates.count(); i++) {
                QStringList coordinateStr = coordinates.at(i).split(",");
                GeographicLib::GeoCoords coordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());

                coordinatesJsonArray.push_back(QString::number(coordinate.Easting(), 'f', 11) + "," + QString::number(coordinate.Northing(), 'f', 11));
            }

            if (boundaryElementName == "outerBoundaryIs" && outerBoundaryJsonArray.isEmpty()) {
                outerBoundaryJsonArray = coordinatesJsonArray;
            } else {
                innerBoundaryJsonArray.push_back(coordinatesJsonArray);
            }
        }
    }

    boundaryFile.close();

    QJsonObject boundaryJsonObject;

    if (!this->isCurrentTaskCanceled()) {
        if (outerBoundaryJsonArray.isEmpty()) {
            throw MeshException("Invalid KML file. No domain coordinates found.");
        }

        boundaryJsonObject["outer"] = outerBoundaryJsonArray;
        boundaryJsonObject["inner"] = innerBoundaryJsonArray;
    }

    return boundaryJsonObject;
}
