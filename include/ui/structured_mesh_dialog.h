#ifndef STRUCTURED_MESH_DIALOG_H
#define STRUCTURED_MESH_DIALOG_H

#include "include/domain/structured_mesh.h"
#include "include/ui/abstract_mesh_dialog.h"

#include <QList>
#include <QDialog>
#include <QWidget>
#include <QSettings>
#include <QShowEvent>
#include <QModelIndex>

namespace Ui {
    class StructuredMeshDialog;
}

class StructuredMeshDialog : public AbstractMeshDialog {
    Q_OBJECT

public:
    explicit StructuredMeshDialog(QWidget *parent = 0);
    virtual ~StructuredMeshDialog();

private slots:
    void on_cbxMeshName_currentTextChanged(const QString &meshName);
    void on_btnNewMesh_clicked();
    void on_btnApplyMesh_clicked();
    void on_btnBoundaryFileBrowser_clicked();
    void on_btnAddIsland_clicked();
    void on_btnRemoveIsland_clicked();
    void on_btnGenerateMesh_clicked();
    void on_btnRemoveMesh_clicked();
    void on_lstIslands_currentTextChanged(const QString &currentText);
    void on_islandItemEdited(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void on_btnClose_clicked();

private:
    const QString BOUNDARY_DEFAULT_DIR_KEY;

    Ui::StructuredMeshDialog *ui;
    StructuredMesh *unsavedMesh;
    StructuredMesh *currentMesh;
    QString currentMeshPolygonName;
    CoordinateSystem coordinateSystem;
};

#endif // STRUCTURED_MESH_DIALOG_H
