#include "include/ui/grid_layer_attributes_dialog.h"
#include "ui_grid_layer_attributes_dialog.h"

#include "include/domain/color_gradient.h"

#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <QDialogButtonBox>
#include <QLinearGradient>
#include <QColorDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QBrush>
#include <QRect>
#include <QPen>

#include "include/ui/grid_data_dialog.h"

GridLayerAttributesDialog::GridLayerAttributesDialog(QWidget *parent, GridData *gridData) :
    QDialog(parent), ui(new Ui::GridLayerAttributesDialog), gridData(gridData)
{
    ui->setupUi(this);
    
    // Map tab setup
    ui->edtMinimum->setText(QString::number(gridData->getMininumRange()));
    ui->edtMaximum->setText(QString::number(gridData->getMaximumRange()));
    
    double *range = gridData->getMesh()->getPolyData()->GetCellData()->GetScalars(gridData->getName().toStdString().c_str())->GetRange();
    this->defaultMinimum = range[0];
    this->defaultMaximum = range[1];
    ui->lblOriginalValues->setText(QString("[%1, %2]").arg(this->defaultMinimum).arg(this->defaultMaximum));
    ui->chkMapLegend->setChecked(gridData->getMapLegend());
    ui->chkLighting->setChecked(gridData->getLighting());
    
    // Line tab setup
    
    // Mesh tab setup
    this->currentLineColor = QColor(gridData->getLineColor());
    QPixmap px(16, 16);
    
    px.fill(currentLineColor);
    ui->btnLineColor->setIcon(px);
    
    Qt::PenStyle styles[2] = { Qt::SolidLine, Qt::DashLine };
    
    for (int i = 0; i < 2; i++) {
        QPixmap pix(120, 14);
        pix.fill(Qt::white);
        
        QBrush brush(Qt::black);
        QPen pen(brush, 2, styles[i]);
        QPainter painter(&pix);
        
        painter.setPen(pen);
        painter.drawLine(2, 8, 118, 8);
        
        ui->cbxLineStyle->setIconSize(QSize(120, 14));
        ui->cbxLineStyle->addItem(QIcon(pix), "");
    }
    
    ui->cbxLineStyle->setCurrentIndex(gridData->getLineStyle() != 0xFFFF);
    
    ui->sbxLineWidth->setValue(gridData->getLineWidth());
    
    int buttonWidth = 60, buttonHeight = 16;
    int row = 0, column = 0;
    
    for (int i = 0; i < ColorGradientTemplate::colorGradients.size(); i++) {
        if (i % 5 == 0) {
            row++;
            column = 0;
        }
        
        QPixmap icon(buttonWidth, buttonHeight);
        QRect rectangle(0, 0, buttonWidth, buttonHeight);
        QList<QColor> colors = ColorGradientTemplate::getColors(i);
        QLinearGradient gradient(rectangle.topLeft(), rectangle.topRight());
        
        for (int c = 0; c < colors.size(); c++) {
            gradient.setColorAt(c / ((double) colors.size() - 1), colors[c]);
        }
        
        QPainter painter(&icon);
        painter.fillRect(rectangle, gradient);
        
        QString templateName = ColorGradientTemplate::getTemplateName(i);
        QToolButton *colorGradientButton = new QToolButton(ui->colorTemplateLayout);
        colorGradientButton->setIcon(icon);
        colorGradientButton->setCheckable(true);
        colorGradientButton->setIconSize(QSize(buttonWidth, buttonHeight));
        colorGradientButton->setToolTip(templateName);
        
        if (templateName == ColorGradientTemplate::defaultTemplateName) {
            this->defaultMapColorGradientButton = colorGradientButton;
        }
        
        if (gridData->getMapColorGradient() == templateName) {
            this->currentMapColorGradientButton = colorGradientButton;
            this->currentMapColorGradientButton->setChecked(true);
        }
        
        QObject::connect(colorGradientButton, SIGNAL(clicked(bool)), this, SLOT(colorGradientButtonClicked(bool)));
        
        static_cast<QGridLayout*>(ui->colorTemplateLayout->layout())->addWidget(colorGradientButton, row, column++);
    }
}

GridLayerAttributesDialog::~GridLayerAttributesDialog() {
    delete ui;
}

void GridLayerAttributesDialog::on_btnUseOriginalValues_clicked() {
    ui->edtMinimum->setText(QString::number(this->defaultMinimum));
    ui->edtMaximum->setText(QString::number(this->defaultMaximum));
}

void GridLayerAttributesDialog::on_btnLineColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this, "Select a line color");
    
    if (color.isValid()) {
        QPixmap px(16, 16);
        px.fill(color);
        
        this->currentLineColor = color;
        ui->btnLineColor->setIcon(px);
    }
}

void GridLayerAttributesDialog::colorGradientButtonClicked(bool checked) {
    if (checked) {
        this->currentMapColorGradientButton->setChecked(false);
        this->currentMapColorGradientButton = static_cast<QToolButton*>(QObject::sender());
    } else {
        this->currentMapColorGradientButton = this->defaultMapColorGradientButton;
        this->currentMapColorGradientButton->setChecked(true);
    }
}

void GridLayerAttributesDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);
    
    if (standardButton == QDialogButtonBox::Cancel) {
        this->reject();
        return;
    }
    
    if (!isValid()) {
        return;
    }
    
    // Map tab
    gridData->setMinimumRange(ui->edtMinimum->text().toDouble());
    gridData->setMaximumRange(ui->edtMaximum->text().toDouble());
    gridData->setMapLegend(ui->chkMapLegend->isChecked());
    gridData->setLighting(ui->chkLighting->isChecked());
    gridData->setMapColorGradient(this->currentMapColorGradientButton->toolTip());
    
    // Points tab
    
    // Mesh tab
    gridData->setLineColor(this->currentLineColor.name());
    gridData->setLineStyle(ui->cbxLineStyle->currentIndex() == 0 ? 0xFFFF : 0xF0F0);
    gridData->setLineWidth(ui->sbxLineWidth->value());
    
    GridDataDialog *gridDataDialog = static_cast<GridDataDialog*>(parentWidget());
    gridDataDialog->getGridDataVTKWidget()->render(gridData);
    
    if (standardButton == QDialogButtonBox::Ok) {
        this->accept();
    }
}

bool GridLayerAttributesDialog::isValid() {
    if (ui->edtMinimum->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Minimum range can't be empty"));
        return false;
    }
    if (ui->edtMaximum->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Maximum range can't be empty"));
        return false;
    }
    
    return true;
}
