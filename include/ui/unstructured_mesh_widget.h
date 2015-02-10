#ifndef UNSTRUCTURED_MESH_WIDGET_H
#define UNSTRUCTURED_MESH_WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QJsonObject>

#include "include/exceptions/mesh_exception.h"
#include "include/domain/observers/mesh_observer.h"
#include "include/application/iph_application.h"

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
    void on_btnGenerateMesh_clicked();
    void on_btnResetMesh_clicked();
    void enableMeshForm(bool enable);
    void enableMeshOptions(bool enable);

    void on_btnEditMesh_clicked();

    void on_cbxMeshName_currentIndexChanged(int index);

private:
    Ui::UnstructuredMeshWidget *ui;    
};

#endif // UNSTRUCTURED_MESH_WIDGET_H
