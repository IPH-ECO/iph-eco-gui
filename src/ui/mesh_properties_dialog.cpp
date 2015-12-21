#include <ui/mesh_properties_dialog.h>
#include "ui_mesh_properties_dialog.h"

#include <QPen>
#include <QBrush>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QColorDialog>
#include <QDialogButtonBox>

MeshPropertiesDialog::MeshPropertiesDialog(QWidget *parent, Mesh *mesh) :
    QDialog(parent), ui(new Ui::MeshPropertiesDialog), mesh(mesh)
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
    
    ui->sbxLineWidth->setValue(mesh->getLineWidth());
    ui->cbxLineStyle->setCurrentIndex(mesh->getLineStyle() != 0xFFFF);
    ui->sldOpacity->setValue(mesh->getOpacity());
    meshColor = mesh->getColor();
}

void MeshPropertiesDialog::on_btnMeshColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this, "Select a color");
    
    if (color.isValid()) {
        QPixmap px(24, 24);
        px.fill(color);
        
        ui->btnMeshColor->setIcon(px);
        meshColor = color.name();
    }
}

void MeshPropertiesDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);
    
	if (standardButton == QDialogButtonBox::Cancel) {
        this->reject();
        return;
    }
    
    mesh->setLineStyle(ui->cbxLineStyle->currentIndex() ? 0xF0F0 : 0xFFFF);
    mesh->setLineWidth(ui->sbxLineWidth->value());
    mesh->setOpacity(ui->sldOpacity->value());
    mesh->setColor(meshColor);

    if (standardButton == QDialogButtonBox::Ok) {
        this->accept();
    }
    
    emit applyChanges(mesh);
}