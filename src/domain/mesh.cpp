#include "include/domain/mesh.h"

Mesh::Mesh(QString &_name) : name(_name) {}

Mesh::Mesh(QString &_name, QString &_boundaryFilePath) : name(_name), boundaryFilePath(_boundaryFilePath) {}

void Mesh::setName(const QString &name) {
    this->name = name;
}

QString Mesh::getName() const {
    return this->name;
}

void Mesh::setBoundaryFilePath(const QString &boundaryFilePath) {
    this->boundaryFilePath = boundaryFilePath;
}

QString Mesh::getBoundaryFilePath() const {
    return this->boundaryFilePath;
}

QJsonObject Mesh::getBoundaryJson() {
//    QFile boundaryFile(this->getBoundaryFilePath());

//    if (!boundaryFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
//        throw MeshException(QString("Unable to open boundary file. Error: %1").arg(boundaryFile.errorString()));
//    }

//    QXmlStreamReader kml(&boundaryFile);
//    QJsonArray outerBoundaryJsonArray, innerBoundaryJsonArray;

//    this->setObservableBoundaryFileSize(boundaryFile.size());

//    while (!kml.atEnd()) {
//        if (this->isCurrentTaskCanceled()) {
//            break;
//        }

//        this->setObservableCurrentBoundaryFileSize(boundaryFile.pos());

//        kml.readNext();

//        if (((kml.name() == "outerBoundaryIs"  && outerBoundaryJsonArray.isEmpty()) || kml.name() == "innerBoundaryIs") && kml.isStartElement()) {
//            QString boundaryElementName = kml.name().toString();

//            do {
//                kml.readNext();
//                this->setObservableCurrentBoundaryFileSize(boundaryFile.pos());
//            } while (kml.name() != "coordinates" && !kml.atEnd());

//            if (kml.atEnd()) {
//                throw MeshException(QString("No boundary domain found."));
//            }

//            // Convert geographic coordinates to UTM coordinates
//            QString coordinatesText = kml.readElementText();
//            QJsonArray coordinatesJsonArray;
//            QStringList coordinates = coordinatesText.trimmed().split(" ");

//            for (int i = 0; i < coordinates.count(); i++) {
//                QStringList coordinateStr = coordinates.at(i).split(",");
//                GeographicLib::GeoCoords coordinate(coordinateStr.at(1).toDouble(), coordinateStr.at(0).toDouble());

//                coordinatesJsonArray.push_back(QString::number(coordinate.Easting(), 'f', 11) + "," + QString::number(coordinate.Northing(), 'f', 11));
//            }

//            if (boundaryElementName == "outerBoundaryIs" && outerBoundaryJsonArray.isEmpty()) {
//                outerBoundaryJsonArray = coordinatesJsonArray;
//            } else {
//                innerBoundaryJsonArray.push_back(coordinatesJsonArray);
//            }
//        }
//    }

//    boundaryFile.close();

    QJsonObject boundaryJsonObject;

//    if (!this->isCurrentTaskCanceled()) {
//        if (outerBoundaryJsonArray.isEmpty()) {
//            throw MeshException("Invalid KML file. No domain coordinates found.");
//        }

//        boundaryJsonObject["outer"] = outerBoundaryJsonArray;
//        boundaryJsonObject["inner"] = innerBoundaryJsonArray;
//    }

    return boundaryJsonObject;
}
