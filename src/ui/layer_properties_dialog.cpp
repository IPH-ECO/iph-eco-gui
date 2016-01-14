#include <ui/layer_properties_dialog.h>
#include "ui_layer_properties_dialog.h"

#include <domain/color_gradient.h>

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
    QDialog(parent),
    ui(new Ui::LayerPropertiesDialog),
    layerProperties(layerProperties),
    tabs(tabs),
    isChanged(false)
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
    
    if (tabs & LayerPropertiesTab::VECTORS) {
        setupVectorsTab();
    } else {
        removeTab(LayerPropertiesTab::VECTORS);
    }
    
    if (tabs & LayerPropertiesTab::MESH) {
        setupMeshTab();
    } else {
        removeTab(LayerPropertiesTab::MESH);
    }

    this->setFixedSize(this->minimumSize());
    this->on_tabWidget_currentChanged(0);
    
    QObject::connect(ui->btnLineColor, SIGNAL(clicked()), this, SLOT(showColorPickerDialog()));
    QObject::connect(ui->btnVectorColor, SIGNAL(clicked()), this, SLOT(showColorPickerDialog()));
}

LayerPropertiesDialog::~LayerPropertiesDialog() {
    delete ui;
}

void LayerPropertiesDialog::setupMapTab() {
    QGridLayout *paletteLayout = static_cast<QGridLayout*>(ui->mapColorTemplateLayout->layout());
    
    ui->chkMapMinimum->setChecked(layerProperties->getUseCustomMapMinimum());
    ui->chkMapMaximum->setChecked(layerProperties->getUseCustomMapMaximum());
    ui->edtMapMinimum->setText(QString::number(layerProperties->getCustomMapMininumRange()));
    ui->edtMapMaximum->setText(QString::number(layerProperties->getCustomMapMaximumRange()));
    this->setupColorGradientTemplates(defaultMapColorGradientButton, currentMapColorGradientButton, paletteLayout);
    ui->chkMapInvertColorTemplate->setChecked(layerProperties->getMapInvertColorGradient());
    ui->sldMapOpacity->setValue(layerProperties->getMapOpacity());
    ui->chkMapLegend->setChecked(layerProperties->getMapLegend());
    ui->chkMapLighting->setChecked(layerProperties->getMapLighting());
}

void LayerPropertiesDialog::setupPointsTab() {
    QGridLayout *paletteLayout = static_cast<QGridLayout*>(ui->pointsColorTemplateLayout->layout());
    
    ui->chkPointsMinimum->setChecked(layerProperties->getUseCustomPointsMinimum());
    ui->chkPointsMaximum->setChecked(layerProperties->getUseCustomPointsMaximum());
    ui->edtPointsMinimum->setText(QString::number(layerProperties->getCustomPointsMininumRange()));
    ui->edtPointsMaximum->setText(QString::number(layerProperties->getCustomPointsMaximumRange()));
    this->setupColorGradientTemplates(defaultPointsColorGradientButton, currentPointsColorGradientButton, paletteLayout);
    ui->chkPointsInvertColorTemplate->setChecked(layerProperties->getPointsInvertColorGradient());
    ui->sldPointsOpacity->setValue(layerProperties->getPointsOpacity());
    ui->spbPointsSize->setValue(layerProperties->getPointsSize());
    ui->chkPointsLegend->setChecked(layerProperties->getPointsLegend());
}

void LayerPropertiesDialog::setupVectorsTab() {
    QGridLayout *paletteLayout = static_cast<QGridLayout*>(ui->vectorsColorTemplateLayout->layout());
    
    ui->chkVectorsMinimum->setChecked(layerProperties->getUseCustomVectorsMinimum());
    ui->chkVectorsMaximum->setChecked(layerProperties->getUseCustomVectorsMaximum());
    ui->edtVectorsMinimum->setText(QString::number(layerProperties->getCustomVectorsMinimumRange()));
    ui->edtVectorsMaximum->setText(QString::number(layerProperties->getCustomVectorsMaximumRange()));
    this->setupColorGradientTemplates(defaultVectorsColorGradientButton, currentVectorsColorGradientButton, paletteLayout);
    ui->chkVectorsInvertColorTemplate->setChecked(layerProperties->getVectorsInvertColorGradient());
    ui->sldVectorsOpacity->setValue(layerProperties->getVectorsOpacity());
    ui->chkVectorsLegend->setChecked(layerProperties->getVectorsLegend());    
    
    QPixmap px(16, 16);
    
    currentVectorColor = QColor(layerProperties->getVectorsColor());
    px.fill(currentVectorColor);
    ui->btnVectorColor->setIcon(px);
    
    ui->edtVectorsScale->setText(QString::number(layerProperties->getVectorsScale()));
    ui->sbxVectorsWidth->setValue(layerProperties->getVectorsWidth());
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

void LayerPropertiesDialog::setupColorGradientTemplates(QToolButton *&defaultButton, QToolButton *&currentButton, QGridLayout *paletteLayout) {
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
        QToolButton *colorGradientButton = new QToolButton(paletteLayout->widget());
        colorGradientButton->setIcon(icon);
        colorGradientButton->setCheckable(true);
        colorGradientButton->setIconSize(QSize(buttonWidth, buttonHeight));
        colorGradientButton->setToolTip(templateName);
        
        if (templateName == ColorGradientTemplate::defaultTemplateName) {
            defaultButton = colorGradientButton;
        }
        
        if (layerProperties->getMapColorGradient() == templateName) {
            currentButton = colorGradientButton;
            currentButton->setChecked(true);
        }
        
        QObject::connect(colorGradientButton, SIGNAL(clicked(bool)), this, SLOT(colorGradientButtonClicked(bool)));
        
        paletteLayout->addWidget(colorGradientButton, row, column++);
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
    QToolButton **currentButton = nullptr;
    QToolButton *defaultButton = nullptr;
    
    if (currentTab == LayerPropertiesTab::MAP) {
        currentButton = &currentMapColorGradientButton;
        defaultButton = defaultMapColorGradientButton;
    } else if (currentTab == LayerPropertiesTab::POINTS) {
        currentButton = &currentPointsColorGradientButton;
        defaultButton = defaultPointsColorGradientButton;
    } else if (currentTab == LayerPropertiesTab::VECTORS) {
        currentButton = &currentVectorsColorGradientButton;
        defaultButton = defaultVectorsColorGradientButton;
    }
    
    isChanged = true;
    
    if (checked) {
        (*currentButton)->setChecked(false);
        *currentButton = static_cast<QToolButton*>(QObject::sender());
    } else {
        *currentButton = defaultButton;
        (*currentButton)->setChecked(true);
    }
}

void LayerPropertiesDialog::on_tabWidget_currentChanged(int index) {
    QString tabText = ui->tabWidget->tabText(index);
    
    if (tabText == "Map") {
        currentTab = LayerPropertiesTab::MAP;
    } else if (tabText == "Points") {
        currentTab = LayerPropertiesTab::POINTS;
    } else if (tabText == "Vectors") {
        currentTab = LayerPropertiesTab::VECTORS;
    } else {
        currentTab = LayerPropertiesTab::MESH;
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
        layerProperties->setCustomMapMinimumRange(ui->edtMapMinimum->text().toDouble());
        layerProperties->setCustomMapMaximumRange(ui->edtMapMaximum->text().toDouble());
        layerProperties->setUseCustomMapMinimum(ui->chkMapMinimum->isChecked());
        layerProperties->setUseCustomMapMaximum(ui->chkMapMaximum->isChecked());
        layerProperties->setMapColorGradient(this->currentMapColorGradientButton->toolTip());
        layerProperties->setMapInvertColorGradient(ui->chkMapInvertColorTemplate->isChecked());
        layerProperties->setMapOpacity(ui->sldMapOpacity->value());
        layerProperties->setMapLegend(ui->chkMapLegend->isChecked());
        layerProperties->setMapLighting(ui->chkMapLighting->isChecked());
    }
    
    // Points tab
    if (tabs & LayerPropertiesTab::POINTS) {
        layerProperties->setCustomPointsMinimumRange(ui->edtPointsMinimum->text().toDouble());
        layerProperties->setCustomPointsMaximumRange(ui->edtPointsMaximum->text().toDouble());
        layerProperties->setUseCustomPointsMinimum(ui->chkPointsMinimum->isChecked());
        layerProperties->setUseCustomPointsMaximum(ui->chkPointsMaximum->isChecked());
        layerProperties->setPointsColorGradient(this->currentPointsColorGradientButton->toolTip());
        layerProperties->setPointsInvertColorGradient(ui->chkPointsInvertColorTemplate->isChecked());
        layerProperties->setPointsOpacity(ui->sldPointsOpacity->value());
        layerProperties->setPointsSize(ui->spbPointsSize->value());
        layerProperties->setPointsLegend(ui->chkPointsLegend->isChecked());
    }
    
    // Vector tab
    if (tabs & LayerPropertiesTab::VECTORS) {
        layerProperties->setCustomVectorsMinimumRange(ui->edtVectorsMinimum->text().toDouble());
        layerProperties->setCustomVectorsMaximumRange(ui->edtVectorsMaximum->text().toDouble());
        layerProperties->setUseCustomVectorsMinimum(ui->chkVectorsMinimum->isChecked());
        layerProperties->setUseCustomVectorsMaximum(ui->chkVectorsMaximum->isChecked());
        layerProperties->setVectorColorMode(ui->chkMagnitude->isChecked() ? VectorColorMode::MAGNITUDE : VectorColorMode::CONSTANT);
        layerProperties->setVectorsColor(this->currentVectorColor.name());
        layerProperties->setVectorsColorGradient(this->currentVectorsColorGradientButton->toolTip());
        layerProperties->setVectorsInvertColorGradient(ui->chkVectorsInvertColorTemplate->isChecked());
        layerProperties->setVectorsOpacity(ui->sldVectorsOpacity->value());
        layerProperties->setVectorsScale(ui->edtVectorsScale->text().toDouble());
        layerProperties->setVectorsWidth(ui->sbxVectorsWidth->value());
        layerProperties->setVectorsLegend(ui->chkVectorsLegend->isChecked());
    }
    
    // Mesh tab
    if (tabs & LayerPropertiesTab::MESH) {
        layerProperties->setMeshLineColor(this->currentLineColor.name());
        layerProperties->setMeshLineStyle(ui->cbxLineStyle->currentIndex() == 0 ? 0xFFFF : 0xF0F0);
        layerProperties->setMeshLineWidth(ui->sbxLineWidth->value());
        layerProperties->setMeshOpacity(ui->sldMeshOpacity->value());
    }
    
    if (isChanged) {
        emit applyChanges();
        isChanged = false;
    }
    
    if (standardButton == QDialogButtonBox::Ok) {
        this->accept();
    }
}

bool LayerPropertiesDialog::isValid() {
    if (tabs & LayerPropertiesTab::MAP) {
        if (ui->chkMapMinimum->isChecked() && ui->edtMapMinimum->text().isEmpty()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Minimum range can't be empty on Map tab."));
            return false;
        }
        if (ui->chkMapMaximum->isChecked() && ui->edtMapMaximum->text().isEmpty()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Maximum range can't be empty on Map tab."));
            return false;
        }
        if (!ui->chkMapMinimum->isChecked() && !ui->chkMapMaximum->isChecked() && ui->edtMapMinimum->text().toDouble() > ui->edtMapMaximum->text().toDouble()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Invalid range on Map tab."));
            return false;
        }
    }
    
    if (tabs & LayerPropertiesTab::POINTS) {
        if (ui->chkPointsMinimum->isChecked() && ui->edtPointsMinimum->text().isEmpty()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Minimum range can't be empty on Points tab."));
            return false;
        }
        if (ui->chkPointsMaximum->isChecked() && ui->edtPointsMaximum->text().isEmpty()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Maximum range can't be empty on Points tab."));
            return false;
        }
        if (!ui->chkPointsMinimum->isChecked() && !ui->chkPointsMaximum->isChecked() && ui->edtPointsMinimum->text().toDouble() > ui->edtPointsMaximum->text().toDouble()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Invalid range on Points tab."));
            return false;
        }
    }
    
    if (tabs & LayerPropertiesTab::VECTORS) {
        if (ui->chkVectorsMinimum->isChecked() && ui->edtVectorsMinimum->text().isEmpty()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Minimum range can't be empty on Vectors tab."));
            return false;
        }
        if (ui->chkVectorsMaximum->isChecked() && ui->edtVectorsMaximum->text().isEmpty()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Maximum range can't be empty on Vectors tab."));
            return false;
        }
        if (!ui->chkVectorsMinimum->isChecked() && !ui->chkVectorsMaximum->isChecked() && ui->edtVectorsMinimum->text().toDouble() > ui->edtVectorsMaximum->text().toDouble()) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Invalid range on Points tab."));
            return false;
        }
        if (ui->edtVectorsScale->text().toDouble() <= 0) {
            QMessageBox::warning(this, tr("Layer Properties"), tr("Vectors scale must be greater than 0."));
            return false;
        }
    }
    
    return true;
}

void LayerPropertiesDialog::changed() {
    this->isChanged = true;
}

void LayerPropertiesDialog::on_cbxLineStyle_currentIndexChanged(int index) {
    this->changed();
}