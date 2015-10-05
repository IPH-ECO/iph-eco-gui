#include "include/ui/grid_layer_attributes_dialog.h"
#include "ui_grid_layer_attributes_dialog.h"

#include "include/domain/color_gradient.h"

#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
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
    
    this->setupMapTab();
    this->setupPointsTab();
    this->setupMeshTab();

    this->setFixedSize(this->sizeHint());
}

GridLayerAttributesDialog::~GridLayerAttributesDialog() {
    delete ui;
}

void GridLayerAttributesDialog::setupMapTab() {
    double *range = gridData->getMesh()->getMeshPolyData()->GetCellData()->GetScalars(gridData->getName().toStdString().c_str())->GetRange();
    this->defaultMapMinimum = range[0];
    this->defaultMapMaximum = range[1];
    
    ui->edtMapMinimum->setText(QString::number(gridData->getMapMininumRange()));
    ui->edtMapMaximum->setText(QString::number(gridData->getMapMaximumRange()));
    ui->lblMapOriginalValues->setText(QString("[%1, %2]").arg(this->defaultMapMinimum).arg(this->defaultMapMaximum));
    this->setupColorGradientTemplates(defaultMapColorGradientButton, currentMapColorGradientButton, true);
    ui->chkMapInvertColorTemplate->setChecked(gridData->getMapInvertColorGradient());
    ui->sldMapOpacity->setValue(gridData->getMapOpacity());
    ui->chkMapLegend->setChecked(gridData->getMapLegend());
    ui->chkMapLighting->setChecked(gridData->getMapLighting());
}

void GridLayerAttributesDialog::setupPointsTab() {
    double *range = gridData->getInputPolyData()->GetPointData()->GetScalars()->GetRange();
    this->defaultPointsMinimum = range[0];
    this->defaultPointsMaximum = range[1];
    
    ui->edtPointsMinimum->setText(QString::number(gridData->getPointsMininumRange()));
    ui->edtPointsMaximum->setText(QString::number(gridData->getPointsMaximumRange()));
    ui->lblPointsOriginalValues->setText(QString("[%1, %2]").arg(this->defaultPointsMinimum).arg(this->defaultPointsMaximum));
    this->setupColorGradientTemplates(defaultPointsColorGradientButton, currentPointsColorGradientButton, false);
    ui->chkPointsInvertColorTemplate->setChecked(gridData->getPointsInvertColorGradient());
    ui->sldPointsOpacity->setValue(gridData->getPointsOpacity());
    ui->spbPointsSize->setValue(gridData->getPointsSize());
    ui->chkPointsLegend->setChecked(gridData->getPointsLegend());
}

void GridLayerAttributesDialog::setupMeshTab() {
    this->currentLineColor = QColor(gridData->getMeshLineColor());
    QPixmap px(16, 16);
    
    px.fill(currentLineColor);
    ui->btnLineColor->setIcon(px);
    
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
    
    ui->cbxLineStyle->setCurrentIndex(gridData->getMeshLineStyle() != 0xFFFF);
    ui->sbxLineWidth->setValue(gridData->getMeshLineWidth());
    ui->sldMeshOpacity->setValue(gridData->getMeshOpacity());
}

void GridLayerAttributesDialog::setupColorGradientTemplates(QToolButton *&defaultButton, QToolButton *&currentButton, bool isMapTab) {
    QGridLayout *layout = static_cast<QGridLayout*>(isMapTab ? ui->mapColorTemplateLayout->layout() : ui->pointsColorTemplateLayout->layout());
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
        QToolButton *colorGradientButton = new QToolButton(layout->widget());
        colorGradientButton->setIcon(icon);
        colorGradientButton->setCheckable(true);
        colorGradientButton->setIconSize(QSize(buttonWidth, buttonHeight));
        colorGradientButton->setToolTip(templateName);
        
        if (templateName == ColorGradientTemplate::defaultTemplateName) {
            defaultButton = colorGradientButton;
        }
        
        if (isMapTab && gridData->getMapColorGradient() == templateName) {
            currentButton = colorGradientButton;
            currentButton->setChecked(true);
        }
        if (!isMapTab && gridData->getPointsColorGradient() == templateName) {
            currentButton = colorGradientButton;
            currentButton->setChecked(true);
        }
        
        QObject::connect(colorGradientButton, SIGNAL(clicked(bool)), this, SLOT(colorGradientButtonClicked(bool)));
        
        layout->addWidget(colorGradientButton, row, column++);
    }
}

void GridLayerAttributesDialog::on_btnUseMapOriginalValues_clicked() {
    ui->edtMapMinimum->setText(QString::number(this->defaultMapMinimum));
    ui->edtMapMaximum->setText(QString::number(this->defaultMapMaximum));
}

void GridLayerAttributesDialog::on_btnUsePointsOriginalValues_clicked() {
    ui->edtPointsMinimum->setText(QString::number(this->defaultPointsMinimum));
    ui->edtPointsMaximum->setText(QString::number(this->defaultPointsMaximum));
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
    bool isMapTab = ui->tabWidget->tabText(ui->tabWidget->tabBar()->currentIndex()) == "Map";
    QToolButton *&currentButton = isMapTab ? currentMapColorGradientButton : currentPointsColorGradientButton;
    QToolButton *defaultButton = isMapTab ? defaultMapColorGradientButton : defaultPointsColorGradientButton;
    
    if (checked) {
        currentButton->setChecked(false);
        currentButton = static_cast<QToolButton*>(QObject::sender());
    } else {
        currentButton = defaultButton;
        currentButton->setChecked(true);
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
    gridData->setMapMinimumRange(ui->edtMapMinimum->text().toDouble());
    gridData->setMapMaximumRange(ui->edtMapMaximum->text().toDouble());
    gridData->setMapColorGradient(this->currentMapColorGradientButton->toolTip());
    gridData->setMapInvertColorGradient(ui->chkMapInvertColorTemplate->isChecked());
    gridData->setMapOpacity(ui->sldMapOpacity->value());
    gridData->setMapLegend(ui->chkMapLegend->isChecked());
    gridData->setMapLighting(ui->chkMapLighting->isChecked());
    
    // Points tab
    gridData->setPointsMinimumRange(ui->edtPointsMinimum->text().toDouble());
    gridData->setPointsMaximumRange(ui->edtPointsMaximum->text().toDouble());
    gridData->setPointsColorGradient(this->currentPointsColorGradientButton->toolTip());
    gridData->setPointsInvertColorGradient(ui->chkPointsInvertColorTemplate->isChecked());
    gridData->setPointsOpacity(ui->sldPointsOpacity->value());
    gridData->setPointsSize(ui->spbPointsSize->value());
    gridData->setPointsLegend(ui->chkPointsLegend->isChecked());
    
    // Mesh tab
    gridData->setMeshLineColor(this->currentLineColor.name());
    gridData->setMeshLineStyle(ui->cbxLineStyle->currentIndex() == 0 ? 0xFFFF : 0xF0F0);
    gridData->setMeshLineWidth(ui->sbxLineWidth->value());
    gridData->setMeshOpacity(ui->sldMeshOpacity->value());
    
    GridDataDialog *gridDataDialog = static_cast<GridDataDialog*>(parentWidget());
    gridDataDialog->getVTKWidget()->render(gridData);
    
    if (standardButton == QDialogButtonBox::Ok) {
        this->accept();
    }
}

bool GridLayerAttributesDialog::isValid() {
    if (ui->edtMapMinimum->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Minimum range can't be empty on Map tab"));
        return false;
    }
    if (ui->edtMapMaximum->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Maximum range can't be empty on Map tab"));
        return false;
    }
    if (ui->edtMapMinimum->text().toDouble() > ui->edtMapMaximum->text().toDouble()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Invalid range on Map tab."));
        return false;
    }
    
    if (ui->edtPointsMinimum->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Minimum range can't be empty on Points tab"));
        return false;
    }
    if (ui->edtPointsMaximum->text().isEmpty()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Maximum range can't be empty on Points tab"));
        return false;
    }
    if (ui->edtPointsMinimum->text().toDouble() > ui->edtPointsMaximum->text().toDouble()) {
        QMessageBox::warning(this, tr("Grid Layer"), tr("Invalid range on Points tab."));
        return false;
    }
    
    return true;
}
