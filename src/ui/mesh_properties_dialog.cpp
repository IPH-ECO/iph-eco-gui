#include "include/ui/mesh_properties_dialog.h"
#include "ui_mesh_properties_dialog.h"

#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QColorDialog>
#include <QDialogButtonBox>

MeshPropertiesDialog::MeshPropertiesDialog(QWidget *parent, Mesh *mesh, MeshVTKWidget *vtkWidget) :
    QDialog(parent), ui(new Ui::MeshPropertiesDialog), vtkWidget(vtkWidget), mesh(mesh)
{
	ui->setupUi(this);
    QColor color(mesh->getColor());
    QPixmap px(24, 24);
    
    px.fill(color);
    ui->btnMeshColor->setIcon(px);

    Qt::PenStyle styles[2] = { Qt::SolidLine, Qt::DashLine };
    
    for (int i = 0; i < 2; i++) {
        QPixmap pix(150, 14);
        pix.fill(Qt::transparent);
        
        QBrush brush(Qt::black);
        QPen pen(brush, 2, styles[i]);
        QPainter painter(&pix);
        
        painter.setPen(pen);
        painter.drawLine(2, 8, 148, 8);
        
        ui->cbxLineStyle->setIconSize(QSize(150, 14));
        ui->cbxLineStyle->addItem(QIcon(pix), "");
    }
    
    ui->sbxLineWidth->blockSignals(true);
    ui->sbxLineWidth->setValue(mesh->getLineWidth());
    ui->sbxLineWidth->blockSignals(false);
    ui->cbxLineStyle->blockSignals(true);
    ui->cbxLineStyle->setCurrentIndex(mesh->getLineStyle() != 0xFFFF);
    ui->cbxLineStyle->blockSignals(false);
    ui->sldOpacity->blockSignals(true);
    ui->sldOpacity->setValue(mesh->getOpacity());
    ui->sldOpacity->blockSignals(false);
}

void MeshPropertiesDialog::on_btnMeshColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this, "Select a color");
    
    if (color.isValid()) {
        QPixmap px(24, 24);
        px.fill(color);
        
        ui->btnMeshColor->setIcon(px);
        mesh->setColor(color.name());
    }
}

void MeshPropertiesDialog::on_cbxLineStyle_currentIndexChanged(int index) {
	mesh->setLineStyle(index ? 0xF0F0 : 0xFFFF);
}

void MeshPropertiesDialog::on_sbxLineWidth_valueChanged(int width) {
	mesh->setLineWidth(width);
}

void MeshPropertiesDialog::on_sldOpacity_valueChanged(int opacity) {
    mesh->setOpacity(opacity);
}

void MeshPropertiesDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);
    
	if (standardButton == QDialogButtonBox::Cancel) {
        this->reject();
        return;
    }

	vtkWidget->render(mesh);

	if (standardButton == QDialogButtonBox::Ok) {
        this->accept();
    }
}