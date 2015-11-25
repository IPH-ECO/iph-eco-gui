#include "include/ui/layer_properties_dialog.h"
#include "ui_layer_properties_dialog.h"

#include "include/domain/color_gradient.h"

#include <QDialogButtonBox>
#include <QLinearGradient>
#include <QColorDialog>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QBrush>
#include <QRect>
#include <QPen>

LayerPropertiesDialog::LayerPropertiesDialog(QWidget *parent, LayerProperties *layerProperties, const LayerPropertiesTab &tabs) :
    QDialog(parent), ui(new Ui::LayerPropertiesDialog), layerProperties(layerProperties), tabs(tabs)
{
    ui->setupUi(this);
    
    if (tabs & LayerPropertiesTab::MAP) {
        setupMapTab();
    } else {
        removeTab(LayerPropertiesTab::MAP);
    }
    
    if (tabs & LayerPropertiesTab::POINTS) {
        setupPointsTab();
    } else {
        removeTab(LayerPropertiesTab::POINTS);
    }
    
    if (tabs & LayerPropertiesTab::MESH) {
        setupMeshTab();
    } else {
        removeTab(LayerPropertiesTab::MESH);
    }
    
    if (tabs & LayerPropertiesTab::VECTORS) {
        setupVectorsTab();
    } else {
        removeTab(LayerPropertiesTab::VECTORS);
    }

    this->setFixedSize(this->sizeHint());
    
    QObject::connect(ui->btnLineColor, SIGNAL(clicked()), this, SLOT(showColorPickerDialog()));
    QObject::connect(ui->btnVectorColor, SIGNAL(clicked()), this, SLOT(showColorPickerDialog()));
    QObject::connect(ui->chkUseMapDefaultValues, SIGNAL(clicked(bool)), layerProperties, SLOT(setUseDefaultMapValues(bool)));
    QObject::connect(ui->chkUsePointsDefaultValues, SIGNAL(clicked(bool)), layerProperties, SLOT(setUseDefaultPointsValues(bool)));
}

LayerPropertiesDialog::~LayerPropertiesDialog() {
    delete ui;
}

void LayerPropertiesDialog::setupMapTab() {
    ui->chkUseMapDefaultValues->setChecked(layerProperties->getUseDefaultMapValues());
    ui->edtMapMinimum->setText(QString::number(layerProperties->getMapMininumRange()));
    ui->edtMapMaximum->setText(QString::number(layerProperties->getMapMaximumRange()));
    ui->edtMapMinimum->setDisabled(ui->chkUseMapDefaultValues->isChecked());
    ui->edtMapMaximum->setDisabled(ui->chkUseMapDefaultValues->isChecked());
    ui->lblMapOriginalValues->setText(QString("[%1, %2]").arg(layerProperties->getDefaultMapMinimum()).arg(layerProperties->getDefaultMapMaximum()));
    this->setupColorGradientTemplates(defaultMapColorGradientButton, currentMapColorGradientButton, true);
    ui->chkMapInvertColorTemplate->setChecked(layerProperties->getMapInvertColorGradient());
    ui->sldMapOpacity->setValue(layerProperties->getMapOpacity());
    ui->chkMapLegend->setChecked(layerProperties->getMapLegend());
    ui->chkMapLighting->setChecked(layerProperties->getMapLighting());
}

void LayerPropertiesDialog::setupPointsTab() {
    ui->chkUsePointsDefaultValues->setChecked(layerProperties->getUseDefaultPointsValues());
    ui->edtPointsMinimum->setText(QString::number(layerProperties->getPointsMininumRange()));
    ui->edtPointsMaximum->setText(QString::number(layerProperties->getPointsMaximumRange()));
    ui->edtPointsMinimum->setDisabled(ui->chkUsePointsDefaultValues->isChecked());
    ui->edtPointsMaximum->setDisabled(ui->chkUsePointsDefaultValues->isChecked());
    ui->lblPointsOriginalValues->setText(QString("[%1, %2]").arg(layerProperties->getDefaultPointsMinimum()).arg(layerProperties->getDefaultPointsMaximum()));
    this->setupColorGradientTemplates(defaultPointsColorGradientButton, currentPointsColorGradientButton, false);
    ui->chkPointsInvertColorTemplate->setChecked(layerProperties->getPointsInvertColorGradient());
    ui->sldPointsOpacity->setValue(layerProperties->getPointsOpacity());
    ui->spbPointsSize->setValue(layerProperties->getPointsSize());
    ui->chkPointsLegend->setChecked(layerProperties->getPointsLegend());
}

void LayerPropertiesDialog::setupMeshTab() {
    this->currentLineColor = QColor(layerProperties->getMeshLineColor());
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
    
    ui->cbxLineStyle->setCurrentIndex(layerProperties->getMeshLineStyle() != 0xFFFF);
    ui->sbxLineWidth->setValue(layerProperties->getMeshLineWidth());
    ui->sldMeshOpacity->setValue(layerProperties->getMeshOpacity());
}

void LayerPropertiesDialog::setupVectorsTab() {
    QPixmap px(16, 16);
    
    currentVectorColor = QColor(layerProperties->getVectorColor());
    px.fill(currentVectorColor);
    ui->btnVectorColor->setIcon(px);
    
    ui->edtVectorScale->setText(QString::number(layerProperties->getVectorScale()));
    ui->sbxVectorWidth->setValue(layerProperties->getVectorWidth());
}

void LayerPropertiesDialog::removeTab(const LayerPropertiesTab &tab) {
    QString tabName;
    
    if (tab == LayerPropertiesTab::MAP) {
        tabName = "mapTab";
    } else if (tab == LayerPropertiesTab::POINTS) {
        tabName = "pointsTab";
    } else if (tab == LayerPropertiesTab::MESH) {
        tabName = "meshTab";
    } else {
        tabName = "vectorsTab";
    }
    
    QWidget *widget = ui->tabWidget->findChild<QWidget*>(tabName);
    int pageIndex = ui->tabWidget->indexOf(widget);
    
    ui->tabWidget->removeTab(pageIndex);
}

void LayerPropertiesDialog::setupColorGradientTemplates(QToolButton *&defaultButton, QToolButton *&currentButton, bool isMapTab) {
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
        
        if (isMapTab && layerProperties->getMapColorGradient() == templateName) {
            currentButton = colorGradientButton;
            currentButton->setChecked(true);
        }
        if (!isMapTab && layerProperties->getPointsColorGradient() == templateName) {
            currentButton = colorGradientButton;
            currentButton->setChecked(true);
        }
        
        QObject::connect(colorGradientButton, SIGNAL(clicked(bool)), this, SLOT(colorGradientButtonClicked(bool)));
        
        layout->addWidget(colorGradientButton, row, column++);
    }
}

void LayerPropertiesDialog::showColorPickerDialog() {
    bool isVectorTab = ui->tabWidget->tabText(ui->tabWidget->tabBar()->currentIndex()) == "Vectors";
    QToolButton *colorButton = isVectorTab ? ui->btnVectorColor : ui->btnLineColor;
    QColor *selectedColor = isVectorTab ? &currentVectorColor : &currentLineColor;
    QColor color = QColorDialog::getColor(Qt::white, this, "Select a color");
    
    if (color.isValid()) {
        QPixmap px(16, 16);
        px.fill(color);
        
        *selectedColor = color;
        colorButton->setIcon(px);
    }
}

void LayerPropertiesDialog::colorGradientButtonClicked(bool checked) {
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

void LayerPropertiesDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);
    
    if (standardButton == QDialogButtonBox::Cancel) {
        this->reject();
        return;
    }
    
    if (!isValid()) {
        return;
    }
    
    // Map tab
    if (tabs & LayerPropertiesTab::MAP) {
        layerProperties->setMapMinimumRange(ui->edtMapMinimum->text().toDouble());
        layerProperties->setMapMaximumRange(ui->edtMapMaximum->text().toDouble());
        layerProperties->setUseDefaultMapValues(ui->chkUseMapDefaultValues->isChecked());
        layerProperties->setMapColorGradient(this->currentMapColorGradientButton->toolTip());
        layerProperties->setMapInvertColorGradient(ui->chkMapInvertColorTemplate->isChecked());
        layerProperties->setMapOpacity(ui->sldMapOpacity->value());
        layerProperties->setMapLegend(ui->chkMapLegend->isChecked());
        layerProperties->setMapLighting(ui->chkMapLighting->isChecked());
    }
    
    // Points tab
    if (tabs & LayerPropertiesTab::POINTS) {
        layerProperties->setPointsMinimumRange(ui->edtPointsMinimum->text().toDouble());
        layerProperties->setPointsMaximumRange(ui->edtPointsMaximum->text().toDouble());
        layerProperties->setUseDefaultPointsValues(ui->chkUsePointsDefaultValues->isChecked());
        layerProperties->setPointsColorGradient(this->currentPointsColorGradientButton->toolTip());
        layerProperties->setPointsInvertColorGradient(ui->chkPointsInvertColorTemplate->isChecked());
        layerProperties->setPointsOpacity(ui->sldPointsOpacity->value());
        layerProperties->setPointsSize(ui->spbPointsSize->value());
        layerProperties->setPointsLegend(ui->chkPointsLegend->isChecked());
    }
    
    // Mesh tab
    if (tabs & LayerPropertiesTab::MESH) {
        layerProperties->setMeshLineColor(this->currentLineColor.name());
        layerProperties->setMeshLineStyle(ui->cbxLineStyle->currentIndex() == 0 ? 0xFFFF : 0xF0F0);
        layerProperties->setMeshLineWidth(ui->sbxLineWidth->value());
        layerProperties->setMeshOpacity(ui->sldMeshOpacity->value());
    }
    
    // Vector tab
    if (tabs & LayerPropertiesTab::VECTORS) {
        layerProperties->setMapLegend(ui->chkLayerLegend->isChecked());
        layerProperties->setVectorColorMode(ui->chkMagnitude->isChecked() ? VectorColorMode::MAGNITUDE : VectorColorMode::CONSTANT);
        layerProperties->setVectorColor(this->currentVectorColor.name());
        layerProperties->setVectorScale(ui->edtVectorScale->text().toDouble());
        layerProperties->setVectorWidth(ui->sbxVectorWidth->value());
    }
    
    emit applyChanges();
    
    if (standardButton == QDialogButtonBox::Ok) {
        this->accept();
    }
}

bool LayerPropertiesDialog::isValid() {
    if (tabs & LayerPropertiesTab::MAP) {
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
    }
    
    if (tabs & LayerPropertiesTab::POINTS) {
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
    }
    
    return true;
}