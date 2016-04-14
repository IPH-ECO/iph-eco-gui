#include <ui/water_quality_dialog.h>
#include "ui_water_quality_dialog.h"

#include <application/iph_application.h>
#include <domain/project.h>
#include <ui/water_quality_parameter_dialog.h>

#include <QUrl>
#include <QLineEdit>
#include <QWebFrame>
#include <QMessageBox>
#include <QWebSettings>

WaterQualityDialog::WaterQualityDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::WaterQualityDialog),
    unsavedConfiguration(new WaterQualityConfiguration),
    currentConfiguration(unsavedConfiguration),
    waterQualityRepository(WaterQualityRepository::getInstance())
{
    QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    
	ui->setupUi(this);
    ui->trwParameters->header()->setStretchLastSection(false);
    ui->trwParameters->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    
    QObject::connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJSObject()));
    
    ui->webView->load(QUrl("qrc:/data/water_quality_diagram.html"));

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
        this->buildParametersTree(rootParameter);
    }
}

WaterQualityDialog::~WaterQualityDialog() {
    delete unsavedConfiguration;
	delete ui;
}

void WaterQualityDialog::buildParametersTree(WaterQualityParameter *parameter) {
    QTreeWidgetItem *parameterItem = nullptr;
    
    if (parameter->getParent()) {
        parameterItem = new QTreeWidgetItem(parameter->getParent()->getItemWidget(), QStringList(parameter->getLabel()));
    } else {
        parameterItem = new QTreeWidgetItem(ui->trwParameters, QStringList(parameter->getLabel()));
    }

    parameter->setItemWidget(parameterItem);
    parameterItem->setExpanded(true);
    parameterItem->setToolTip(0, parameter->getDescription());
    parameterItem->setData(0, Qt::UserRole, QVariant(parameter->getName()));
    parameterItem->setData(1, Qt::UserRole, QVariant(parameter->getDiagramItem()));
    
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
        toolButton->setEnabled(parameter->isEnabled());
        toolButton->setIcon(QIcon(":/icons/view-form-table.png"));
        toolButton->setFixedSize(18, 18);
        
        QObject::connect(toolButton, SIGNAL(clicked()), this, SLOT(onTabularInputButtonClicked()));
        
        QHBoxLayout *layout = new QHBoxLayout();
        
        layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
        layout->addWidget(toolButton);
        layout->setMargin(0);
        
        QWidget *buttonWidget = new QWidget(ui->trwParameters);
        
        buttonWidget->setLayout(layout);
        
        ui->trwParameters->setItemWidget(parameterItem, 1, buttonWidget);
    }
    
    for (WaterQualityParameter *child : parameter->getChildren()) {
        buildParametersTree(child);
    }
}

void WaterQualityDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    if (!configurationName.isEmpty()) {
        currentConfiguration = IPHApplication::getCurrentProject()->getWaterQualityConfiguration(configurationName);
        ui->edtConfigurationName->setText(currentConfiguration->getName());
        ui->cbxGridDataConfiguration->setCurrentText(currentConfiguration->getGridDataConfiguration()->getName());
    }
    
//    this->buildParametersTree();
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
    if (item->checkState(0) == Qt::Unchecked) {
        ui->webView->page()->mainFrame()->evaluateJavaScript( "disableVariable('" + item->data(1, Qt::UserRole).toString() + "'); null" );
    } else if (item->checkState(0) == Qt::Checked) {
        ui->webView->page()->mainFrame()->evaluateJavaScript( "enableVariable('" + item->data(1, Qt::UserRole).toString() + "'); null" );
    }
}

void WaterQualityDialog::addJSObject() {
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("waterQualityDialog", this);
}

void WaterQualityDialog::toggleItem(const QString &itemName, const bool &checked) {
    QTreeWidgetItemIterator it(ui->trwParameters, QTreeWidgetItemIterator::All);
    
    ui->trwParameters->blockSignals(true);
    while (*it) {
        if ((*it)->data(1, Qt::UserRole).toString() == itemName) {
            (*it)->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
            break;
        }
        it++;
    }
    ui->trwParameters->blockSignals(false);
}

void WaterQualityDialog::onTabularInputButtonClicked() {
	QToolButton *button = static_cast<QToolButton*>(sender());
    WaterQualityParameter *parameter = waterQualityRepository->findParameterByName(button->objectName());
    QList<int> groupsDistribuition;
    
    for (QString groupName : parameter->getGroups()) {
        WaterQualityParameter *groupParameter = waterQualityRepository->findParameterByName(groupName);
        int groupValue = ui->trwParameters->findChild<QLineEdit*>(groupName)->text().toInt();
        
        if (!groupParameter->isInRange(groupValue)) {
            QMessageBox::warning(this, "Water Quality", QString("Group %1 must be between %2 and %3.").arg(groupParameter->getLabel()).arg(groupParameter->getRangeMinimum()).arg(groupParameter->getRangeMaximum()));
            return;
        }

        groupsDistribuition << groupValue;
    }

    WaterQualityParameterDialog dialog(this, parameter, groupsDistribuition);
    dialog.exec();
}