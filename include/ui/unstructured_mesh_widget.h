#ifndef UNSTRUCTURED_MESH_WIDGET_H
#define UNSTRUCTURED_MESH_WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "include/services/mesh_service.h"

namespace Ui {
class UnstructuredMeshWidget;
}

class UnstructuredMeshWidget : public QWidget {
    Q_OBJECT

public:
    explicit UnstructuredMeshWidget(QWidget *parent = 0);
    ~UnstructuredMeshWidget();

private slots:
    void on_btnNewMesh_clicked();
    void on_btnRemoveMesh_clicked();
    void on_btnBoundaryFileBrowser_clicked();
    void on_btnResetMesh_clicked();
    void toggleFormButtons(bool enable);

private:
    Ui::UnstructuredMeshWidget *ui;
    MeshService *meshService;
};

#endif // UNSTRUCTURED_MESH_WIDGET_H