#include <ui/water_quality_dialog.h>
#include "ui_water_quality_dialog.h"

#include <application/iph_application.h>
#include <domain/project.h>

#include <QUrl>
#include <QLineEdit>
#include <QMessageBox>

WaterQualityDialog::WaterQualityDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::WaterQualityDialog),
    unsavedConfiguration(new WaterQualityConfiguration),
    currentConfiguration(unsavedConfiguration),
    waterQualityRepository(WaterQualityRepository::getInstance())
{
	ui->setupUi(this);
    ui->trwParameters->header()->setStretchLastSection(false);
    ui->trwParameters->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->webView->setUrl(QUrl("qrc:/data/water_quality_diagram.html"));

    Project *project = IPHApplication::getCurrentProject();
    
    ui->cbxConfiguration->blockSignals(true);
    for (WaterQualityConfiguration *configuration : project->getWaterQualityConfigurations()) {
        ui->cbxConfiguration->addItem(configuration->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxConfiguration->blockSignals(false);

    QSet<GridDataConfiguration*> gridDataConfigurations = project->getGridDataConfigurations();

    ui->cbxGridDataConfiguration->blockSignals(true);
    for (GridDataConfiguration *configuration : project->getGridDataConfigurations()) {
        ui->cbxGridDataConfiguration->addItem(configuration->getName());
    }
    ui->cbxGridDataConfiguration->setCurrentIndex(-1);
    ui->cbxGridDataConfiguration->blockSignals(false);

    for (WaterQualityParameter *rootParameter : waterQualityRepository->getRootParameters()) {
        this->setupItems(rootParameter);
    }
}

WaterQualityDialog::~WaterQualityDialog() {
    delete unsavedConfiguration;
	delete ui;
}

void WaterQualityDialog::setupItems(WaterQualityParameter *parameter) {
    QTreeWidgetItem *parameterItem = nullptr;
    
    if (parameter->getParent()) {
        parameterItem = new QTreeWidgetItem(parameter->getParent()->getItemWidget(), QStringList(parameter->getLabel()));
    } else {
        parameterItem = new QTreeWidgetItem(ui->trwParameters, QStringList(parameter->getLabel()));
    }

    parameter->setItemWidget(parameterItem);
    parameterItem->setData(0, Qt::UserRole, QVariant(parameter->getName()));
    
    if (parameter->isCheckable()) {
        parameterItem->setCheckState(0, parameter->isChecked() ? Qt::Checked : Qt::Unchecked);
    }
    
    if (parameter->getInputType() == WaterQualityParameterInputType::INLINE) {
        QLineEdit *lineEdit = new QLineEdit(ui->trwParameters);
        
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setEnabled(parameter->isEnabled());
        lineEdit->setObjectName(parameter->getName());
        lineEdit->setText(QString::number(parameter->getValue()));
        
        ui->trwParameters->setItemWidget(parameterItem, 1, lineEdit);
    } else if (parameter->getInputType() == WaterQualityParameterInputType::TABULAR) {
        QToolButton *toolButton = new QToolButton(ui->trwParameters);
        
        toolButton->setObjectName(parameter->getName());
        toolButton->setText("");
        toolButton->setEnabled(parameter->isEnabled());
        toolButton->setIcon(QIcon(":/icons/view-form-table.png"));
        toolButton->setFixedSize(18, 18);
        
        QHBoxLayout *layout = new QHBoxLayout();
        
        layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
        layout->addWidget(toolButton);
        layout->setMargin(0);
        
        QWidget *buttonWidget = new QWidget(ui->trwParameters);
        
        buttonWidget->setLayout(layout);
        
        ui->trwParameters->setItemWidget(parameterItem, 1, buttonWidget);
    }
    
    for (WaterQualityParameter *child : parameter->getChildren()) {
        setupItems(child);
    }
}

void WaterQualityDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    if (!configurationName.isEmpty()) {
        currentConfiguration = IPHApplication::getCurrentProject()->getWaterQualityConfiguration(configurationName);
        ui->edtConfigurationName->setText(currentConfiguration->getName());
        ui->cbxGridDataConfiguration->setCurrentText(currentConfiguration->getGridDataConfiguration()->getName());
    }
    
//    this->setupItems();
}

void WaterQualityDialog::on_cbxGridDataConfiguration_currentIndexChanged(const QString &gridDataConfigurationName) {
    if (gridDataConfigurationName.isEmpty()) {
        return;
    }
    
    GridDataConfiguration *gridDataConfiguration = IPHApplication::getCurrentProject()->getGridDataConfiguration(gridDataConfigurationName);
    currentConfiguration->setGridDataConfiguration(gridDataConfiguration);
}

void WaterQualityDialog::on_btnRemoveConfiguration_clicked() {
    QMessageBox::StandardButton question = QMessageBox::question(this, tr("Water Quality"), tr("Are you sure you want to remove the selected configuration?"));
    
    if (question == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeWaterQualityConfiguration(ui->cbxConfiguration->currentText());
        ui->cbxConfiguration->removeItem(ui->cbxConfiguration->currentIndex());
        this->on_btnNewConfiguration_clicked();
    }
}

void WaterQualityDialog::on_btnNewConfiguration_clicked() {
    currentConfiguration = unsavedConfiguration;
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxGridDataConfiguration->setCurrentIndex(-1);
    ui->edtConfigurationName->setText("");
    ui->edtConfigurationName->setFocus();
}

void WaterQualityDialog::on_btnApplyConfiguration_clicked() {
    QString oldConfigurationName = ui->cbxConfiguration->currentText();
    QString newConfigurationName = ui->edtConfigurationName->text();
    
    if (newConfigurationName.isEmpty()) {
        QMessageBox::warning(this, tr("Water Quality"), tr("Configuration name can't be empty."));
        return;
    }
    
    if (ui->cbxGridDataConfiguration->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Water Quality"), tr("Grid data configuration can't be empty."));
        return;
    }
    
    QList<WaterQualityParameter*> parameters = currentConfiguration->getParameters();
    
    for (WaterQualityParameter *parameter : currentConfiguration->getParameters()) {
        
    }
    
    currentConfiguration->setName(newConfigurationName);

    ui->cbxConfiguration->blockSignals(true);
    if (oldConfigurationName.isEmpty()) { // new configuration
        IPHApplication::getCurrentProject()->addWaterQualityConfiguration(currentConfiguration);
        unsavedConfiguration = new WaterQualityConfiguration();

        ui->cbxConfiguration->addItem(newConfigurationName);
        ui->cbxConfiguration->setCurrentText(newConfigurationName);
    } else {
        ui->cbxConfiguration->setItemText(ui->cbxConfiguration->currentIndex(), newConfigurationName);
    }
    ui->cbxConfiguration->blockSignals(false);
}

void WaterQualityDialog::on_trwParameters_itemChanged(QTreeWidgetItem *item, int column) {
    
}