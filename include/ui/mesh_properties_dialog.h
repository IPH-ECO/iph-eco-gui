#ifndef MESH_PROPERTIES_DIALOG_H
#define MESH_PROPERTIES_DIALOG_H

#include <domain/mesh.h>
#include <ui/mesh_vtk_widget.h>

#include <QColor>
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
    QString meshColor;
    Mesh *mesh;
private slots:
    void on_btnMeshColor_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
public:
	explicit MeshPropertiesDialog(QWidget *parent, Mesh *mesh);
signals:
    void applyChanges(Mesh *mesh);
};

#endif // MESH_PROPERTIES_DIALOG_H