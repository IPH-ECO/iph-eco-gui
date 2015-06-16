#ifndef STRUCTURED_MESH_DIALOG_H
#define STRUCTURED_MESH_DIALOG_H

#include "include/domain/structured_mesh.h"

#include <QDialog>
#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSettings>
#include <QModelIndex>

namespace Ui {
    class StructuredMeshDialog;
}

class StructuredMeshDialog : public QDialog {
    Q_OBJECT

public:
    explicit StructuredMeshDialog(QWidget *parent = 0);
    ~StructuredMeshDialog();

    void setArea(const double &area);

public slots:
    void setCoordinate(double &x, double &y);

private slots:
    void on_btnBoundaryFileBrowser_clicked();
    void on_btnAddIsland_clicked();
    void on_btnRemoveIsland_clicked();
    void on_btnGenerateMesh_clicked();
    void on_btnSaveMesh_clicked();
    void on_btnCancelMesh_clicked();
    void on_btnRemoveMesh_clicked();
    void on_cbxMeshName_currentIndexChanged(int index);
    void on_lstIslands_currentTextChanged(const QString &currentText);
    void on_islandItemEdited(const QModelIndex &topLeft, const QModelIndex &bottomRight);

private:
    const QString BOUNDARY_DEFAULT_DIR_KEY;

    Ui::StructuredMeshDialog *ui;
    StructuredMesh *unsavedMesh;
    StructuredMesh *currentMesh;
    QSettings *appSettings;
    QString currentMeshPolygonName;

    QString getDefaultDirectory();
    void enableMeshForm(bool enable);
    void resetMeshForm();
};

#endif // STRUCTURED_MESH_DIALOG_H
