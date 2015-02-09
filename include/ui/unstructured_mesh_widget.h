#ifndef UNSTRUCTURED_MESH_WIDGET_H
#define UNSTRUCTURED_MESH_WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QJsonObject>

#include "include/services/unstructured_mesh_service.h"
#include "include/domain/observers/mesh_observer.h"

namespace Ui {
class UnstructuredMeshWidget;
}

class UnstructuredMeshWidget : public QWidget {
    Q_OBJECT

public:
    explicit UnstructuredMeshWidget(QWidget *parent = 0);
    ~UnstructuredMeshWidget();

private slots:
    void on_newMeshButton_clicked();
    void on_boundaryFileBrowserButton_clicked();
    void on_resetMeshButton_clicked();
    void on_generateMeshButton_clicked();

private:
    Ui::UnstructuredMeshWidget *ui;
    MeshService *meshService;
    QString currentMeshName;
};

#endif // UNSTRUCTURED_MESH_WIDGET_H
