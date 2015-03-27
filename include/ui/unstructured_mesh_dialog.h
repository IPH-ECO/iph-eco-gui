#ifndef UNSTRUCTURED_MESH_DIALOG_H
#define UNSTRUCTURED_MESH_DIALOG_H

#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <QSettings>
#include <QJsonObject>

#include "include/application/iph_application.h"
#include "include/domain/unstructured_mesh.h"
#include "include/exceptions/mesh_exception.h"

namespace Ui {
class UnstructuredMeshDialog;
}

class UnstructuredMeshDialog : public QDialog {
    Q_OBJECT

public:
    explicit UnstructuredMeshDialog(QWidget *parent = 0);
    ~UnstructuredMeshDialog();

private slots:
    // General dialog events
    void on_btnRemoveMesh_clicked();
    void on_btnSaveMesh_clicked();
    void on_btnCancelMesh_clicked();
    void on_cbxMeshName_currentIndexChanged(int index);
    void on_chkShowDomainBoundary_clicked();

    // Domain tab events
    void on_btnGenerateDomain_clicked();
    void on_btnBoundaryFileBrowser_clicked();

    // Mesh tab events
    void on_btnGenerateMesh_clicked();
    void on_btnAddCoordinatesFile_clicked();
    void on_btnRemoveCoordinatesFile_clicked();
    void on_lstCoordinateFiles_itemSelectionChanged();
    void on_sbxMaximumEdgeLength_valueChanged(double value);
    void on_sbxMinimumAngle_valueChanged(double value);
    void on_chkShowMesh_clicked();

    void resetMeshForm();
    void enableMeshForm(bool enable);

private:
    const QString BOUNDARY_DEFAULT_DIR_KEY;

    Ui::UnstructuredMeshDialog *ui;
    UnstructuredMesh *unsavedMesh;
    UnstructuredMesh *currentMesh;
    QSettings *appSettings;

    QString getDefaultDirectory();
};

#endif // UNSTRUCTURED_MESH_DIALOG_H
