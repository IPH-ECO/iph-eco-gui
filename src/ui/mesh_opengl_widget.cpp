#include "include/ui/mesh_opengl_widget.h"

MeshOpenGLWidget::MeshOpenGLWidget(QWidget *parent) : QOpenGLWidget(parent), minWidth(0.0), maxWidth(0.0), minHeight(0.0), maxHeight(0.0) {
    this->setEnabled(false); //Ensures that widget is disabled at initialization
}

void MeshOpenGLWidget::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void MeshOpenGLWidget::resizeGL(int width, int height) {
    glViewport(0, 0, width, height);
}

void MeshOpenGLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!this->isEnabled()) {
        return;
    }

    if (this->domainCoordinates.empty()) {
        QJsonObject boundaryJson = mesh.getBoundaryJson();
        QJsonArray domainCoordinatesJson = boundaryJson["domain"].toArray();
        QJsonArray holesJson = boundaryJson["holes"].toArray();

        parseDomainCoordinates(domainCoordinatesJson);

        for (int i = 0; i < holesJson.count(); i++) {
            QJsonArray holeCoordinates = holesJson.at(i).toArray();
            parseHoleCoordinates(holeCoordinates);
        }
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(this->minWidth, this->maxWidth, this->minHeight, this->maxHeight, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);

    glColor3f(0.0, 0.0, 0.0);
    glPointSize(1.0f);

    if (!mesh.getShowDomainBoundary()) {
        return;
    }

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < this->domainCoordinates.count() - 1; i++) {
        QPointF p1 = this->domainCoordinates.at(i);
        QPointF p2 = this->domainCoordinates.at(i + 1);

        glVertex2d(p1.x(), p1.y());
        glVertex2d(p2.x(), p2.y());
    }
    glEnd();

    for (int i = 0; i < this->holes.count(); i++) {
        QVector<QPointF> holeCoordinates = this->holes.at(i);

        glBegin(GL_LINE_LOOP);
        for (int j = 0; j < holeCoordinates.count() - 1; j++) {
            QPointF p1 = holeCoordinates.at(j);
            QPointF p2 = holeCoordinates.at(j + 1);

            glVertex2d(p1.x(), p1.y());
            glVertex2d(p2.x(), p2.y());
        }
        glEnd();
    }
}

void MeshOpenGLWidget::showDomain(const Mesh &mesh) {
    this->mesh = mesh;
    this->setEnabled(true);
}

void MeshOpenGLWidget::clearMap() {
    this->setEnabled(false);
    this->domainCoordinates.clear();
    this->holes.clear();
}

void MeshOpenGLWidget::toogleBoundaryDomain(bool show) {
    this->mesh.setShowDomainBoundary(show);
}

void MeshOpenGLWidget::parseDomainCoordinates(QJsonArray &jsonCoordinates) {
    for (int i = 0; i < jsonCoordinates.count(); i++) {
        QJsonObject jsonCoordinate = jsonCoordinates.at(i).toObject();
        double x = jsonCoordinate["x"].toDouble();
        double y = jsonCoordinate["y"].toDouble();

        this->domainCoordinates.push_back(QPointF(x, y));

        if (this->minWidth == 0.0 || x < this->minWidth) {
            this->minWidth = x;
        }
        if (this->maxWidth == 0.0 || x > this->maxWidth) {
            this->maxWidth = x;
        }

        if (this->minHeight == 0.0 || y < this->minHeight) {
            this->minHeight = y;
        }
        if (this->maxHeight == 0.0 || y > this->maxHeight) {
            this->maxHeight = y;
        }
    }
}

void MeshOpenGLWidget::parseHoleCoordinates(QJsonArray &jsonCoordinates) {
    QVector<QPointF> coordinates;

    for (int i = 0; i < jsonCoordinates.count(); i++) {
        QJsonObject jsonCoordinate = jsonCoordinates.at(i).toObject();
        double x = jsonCoordinate["x"].toDouble();
        double y = jsonCoordinate["y"].toDouble();

        coordinates.push_back(QPointF(x, y));
    }

    this->holes.push_back(coordinates);
}

void MeshOpenGLWidget::wheelEvent(QWheelEvent *event) {
    qDebug() << "zoom in/out";
}

void MeshOpenGLWidget::mouseMoveEvent(QMouseEvent *event) {
    qDebug() << "move";
}

void MeshOpenGLWidget::mousePressEvent(QMouseEvent *event) {
    qDebug() << "press";
}

void MeshOpenGLWidget::mouseReleaseEvent(QMouseEvent *event) {
    qDebug() << "release";
}
