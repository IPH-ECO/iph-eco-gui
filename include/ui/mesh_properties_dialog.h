#ifndef MESH_PROPERTIES_DIALOG_H
#define MESH_PROPERTIES_DIALOG_H

#include "include/domain/mesh.h"
#include "include/ui/mesh_vtk_widget.h"

#include <QDialog>
#include <QWidget>
#include <QAbstractButton>

namespace Ui {
	class MeshPropertiesDialog;
}

class MeshPropertiesDialog : public QDialog {
	Q_OBJECT
private:
	Ui::MeshPropertiesDialog *ui;
    MeshVTKWidget *vtkWidget;
    Mesh *mesh;
private slots:
    void on_btnMeshColor_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_cbxLineStyle_currentIndexChanged(int index);
    void on_sldOpacity_valueChanged(int opacity);
    void on_sbxLineWidth_valueChanged(int width);
public:
	explicit MeshPropertiesDialog(QWidget *parent, Mesh *mesh);
signals:
    void applyChanges(Mesh *mesh);
};

#endif // MESH_PROPERTIES_DIALOG_H