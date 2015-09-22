#ifndef STRUCTURED_MESH_DIALOG_H
#define STRUCTURED_MESH_DIALOG_H

#include "include/domain/structured_mesh.h"

#include <QList>
#include <QDialog>
#include <QWidget>
#include <QSettings>
#include <QShowEvent>
#include <QModelIndex>

namespace Ui {
    class StructuredMeshDialog;
}

class StructuredMeshDialog : public QDialog {
    Q_OBJECT

public:
    explicit StructuredMeshDialog(QWidget *parent = 0);
    ~StructuredMeshDialog();

public slots:
    void setCoordinate(double &x, double &y);

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
    void onToggleLabelsClicked(bool show);
    void onChangeBackgroundClicked();
    void onExportMapClicked();
    void onMeshPropertiesClicked();

private:
    const QString BOUNDARY_DEFAULT_DIR_KEY;

    Ui::StructuredMeshDialog *ui;
    StructuredMesh *unsavedMesh;
    StructuredMesh *currentMesh;
    QSettings *appSettings;
    QString currentMeshPolygonName;
    QList<QAction*> toolBarActions;
    QAction *toggleVerticeLabelsAction;
    QAction *toggleCellLabelsAction;
    QAction *changeBackgroundAction;
    CoordinateSystem coordinateSystem;

    QString getDefaultDirectory();
    
    virtual void showEvent(QShowEvent *event);
};

#endif // STRUCTURED_MESH_DIALOG_H
