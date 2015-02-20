#ifndef UNSTRUCTURED_MESH_DIALOG_H
#define UNSTRUCTURED_MESH_DIALOG_H

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
class UnstructuredMeshDialog;
}

class UnstructuredMeshDialog : public QDialog {
    Q_OBJECT

public:
    explicit UnstructuredMeshDialog(QWidget *parent = 0);
    ~UnstructuredMeshDialog();

private slots:
    void on_btnRemoveMesh_clicked();
    void on_btnBoundaryFileBrowser_clicked();
    void on_btnGenerateMesh_clicked();
    void on_btnResetMesh_clicked();
    void on_btnSaveMesh_clicked();
    void on_btnCancelMesh_clicked();
    void resetMeshForm();
    void enableMeshForm(bool enable);

    void on_cbxMeshName_currentIndexChanged(int index);

    // Options
    void on_chkShowDomainBoundary_clicked();

private:
    Ui::UnstructuredMeshDialog *ui;
};

#endif // UNSTRUCTURED_MESH_DIALOG_H
