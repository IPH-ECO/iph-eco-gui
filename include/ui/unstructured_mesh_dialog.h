#ifndef UNSTRUCTURED_MESH_DIALOG_H
#define UNSTRUCTURED_MESH_DIALOG_H

#include "include/domain/unstructured_mesh.h"
#include <QDialog>
#include <QWidget>
#include <QSettings>
#include <QModelIndex>

namespace Ui {
class UnstructuredMeshDialog;
}

class UnstructuredMeshDialog : public QDialog {
    Q_OBJECT

public:
    explicit UnstructuredMeshDialog(QWidget *parent = 0);
    ~UnstructuredMeshDialog();
    
    void setArea(const double &area);

public slots:
    void setCoordinate(double &x, double &y);

private slots:
    // Domain tab events
    void on_btnBoundaryFileBrowser_clicked();
    void on_islandItemEdited(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void on_lstIslands_currentTextChanged(const QString &currentText);
    void on_btnAddIsland_clicked();
    void on_btnRemoveIsland_clicked();
    void on_btnGenerateMesh1_clicked();

    // Mesh tab events
    void on_lstRefinementAreas_itemSelectionChanged();
    void on_btnAddCoordinatesFile_clicked();
    void on_btnRemoveCoordinatesFile_clicked();
    void on_sbxMaximumEdgeLength_valueChanged(double value);
    void on_sbxMinimumAngle_valueChanged(double value);
    void on_btnGenerateMesh2_clicked();

    // General dialog events
    void on_cbxMeshName_currentIndexChanged(int index);
    void on_btnSaveMesh_clicked();
    void on_btnCancelMesh_clicked();
    void on_btnRemoveMesh_clicked();

private:
    const QString BOUNDARY_DEFAULT_DIR_KEY;

    Ui::UnstructuredMeshDialog *ui;
    UnstructuredMesh *unsavedMesh;
    UnstructuredMesh *currentMesh;
    QSettings *appSettings;
    QString currentIslandName;
    QString currentRefinementAreaName;
    CoordinateSystem meshCoordinateSystem;

    QString getDefaultDirectory();
    void resetMeshForm();
    void enableMeshForm(bool enable);
};

#endif // UNSTRUCTURED_MESH_DIALOG_H
