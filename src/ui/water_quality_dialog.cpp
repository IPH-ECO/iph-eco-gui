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
    ui->setupUi(this);
    ui->trwParameter->header()->setStretchLastSection(false);
    ui->trwParameter->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->trwStructure->header()->setStretchLastSection(false);
    ui->trwStructure->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    
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
}

void WaterQualityDialog::on_webView_loadFinished(bool ok) {
    for (WaterQualityParameter *rootParameter : waterQualityRepository->getRootParameters()) {
        this->buildParameterTree(rootParameter);
    }
}

WaterQualityDialog::~WaterQualityDialog() {
    delete unsavedConfiguration;
	delete ui;
}

void WaterQualityDialog::buildParameterTree(WaterQualityParameter *parameter) {
    QTreeWidget *treeWidget = parameter->getSection() == WaterQualityParameterSection::STRUCTURE ? ui->trwStructure : ui->trwParameter;
    QTreeWidgetItem *parameterItem = nullptr;
    
    if (parameter->getParent()) {
        parameterItem = new QTreeWidgetItem(parameter->getParent()->getItemWidget(), QStringList(parameter->getLabel()));
    } else {
        parameterItem = new QTreeWidgetItem(treeWidget, QStringList(parameter->getLabel()));
    }

    parameter->setItemWidget(parameterItem);
    
    ui->trwStructure->blockSignals(true);
    parameterItem->setToolTip(0, parameter->getDescription());
    parameterItem->setHidden(!parameter->isEnabled());
    parameterItem->setData(0, Qt::UserRole, QVariant(parameter->getName()));
    parameterItem->setData(1, Qt::UserRole, QVariant(parameter->getDiagramItem()));
    ui->trwStructure->blockSignals(false);
    
    if (parameter->isCheckable()) {
        parameterItem->setCheckState(0, parameter->isChecked() ? Qt::Checked : Qt::Unchecked);
    }
    
    if (parameter->getInputType() == WaterQualityParameterInputType::INLINE) {
        QLineEdit *lineEdit = new QLineEdit(treeWidget);
        
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setEnabled(parameter->isEnabled());
        lineEdit->setObjectName(parameter->getName());
        lineEdit->setText(QString::number(parameter->getValue()));
        
        treeWidget->setItemWidget(parameterItem, 1, lineEdit);
    } else if (parameter->getInputType() == WaterQualityParameterInputType::TABULAR) {
        QToolButton *toolButton = new QToolButton(treeWidget);
        
        toolButton->setObjectName(parameter->getName());
        toolButton->setEnabled(parameter->isEnabled());
        toolButton->setIcon(QIcon(":/icons/view-form-table.png"));
        toolButton->setFixedSize(18, 18);
        
        QObject::connect(toolButton, SIGNAL(clicked()), this, SLOT(onTabularInputButtonClicked()));
        
        QHBoxLayout *layout = new QHBoxLayout();
        
        layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
        layout->addWidget(toolButton);
        layout->setMargin(0);
        
        QWidget *buttonWidget = new QWidget(treeWidget);
        
        buttonWidget->setLayout(layout);
        
        treeWidget->setItemWidget(parameterItem, 1, buttonWidget);
    }
    
    for (WaterQualityParameter *child : parameter->getChildren()) {
        buildParameterTree(child);
    }
}

void WaterQualityDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    if (!configurationName.isEmpty()) {
        currentConfiguration = IPHApplication::getCurrentProject()->getWaterQualityConfiguration(configurationName);
        ui->edtConfigurationName->setText(currentConfiguration->getName());
        ui->cbxGridDataConfiguration->setCurrentText(currentConfiguration->getGridDataConfiguration()->getName());
    }
    
//    this->buildParameterTree();
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
    
    for (WaterQualityParameter *parameter : parameters) {
        
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

void WaterQualityDialog::on_trwStructure_itemChanged(QTreeWidgetItem *item, int column) {
    WaterQualityParameter *sourceParameter = waterQualityRepository->findParameterByName(item->data(0, Qt::UserRole).toString(), WaterQualityParameterSection::STRUCTURE);
    
    if (sourceParameter && (item->checkState(0) == Qt::Checked || item->checkState(0) == Qt::Unchecked)) {
        bool isSourceChecked = item->checkState(0) == Qt::Checked;
        QString jsFunction = isSourceChecked ? "enableVariable" : "disableVariable";
        
        ui->webView->page()->mainFrame()->evaluateJavaScript(jsFunction + "('" + sourceParameter->getDiagramItem() + "'); null");
    
        WaterQualityParameter *targetParameter = waterQualityRepository->findParameterByName(sourceParameter->getTarget(), WaterQualityParameterSection::PARAMETER);
    
        if (targetParameter && targetParameter->getItemWidget()) {
            targetParameter->getItemWidget()->setHidden(!isSourceChecked);
        }
        
        if (isSourceChecked) {
            WaterQualityParameter *parent = sourceParameter->getParent();
            
            if (parent && parent->isCheckable()) {
                QTreeWidgetItem *parentItem = parent->getItemWidget();
                
                if (parentItem->checkState(0) == Qt::Unchecked) {
                    parentItem->setCheckState(0, Qt::Checked);
                }
            }
        } else {
            for (WaterQualityParameter *child : sourceParameter->getChildren()) {
                QTreeWidgetItem *childItem = child->getItemWidget();
                
                if (childItem && childItem->checkState(0) == Qt::Checked) {
                    childItem->setCheckState(0, Qt::Unchecked);
                }
            }
        }
    }
}

void WaterQualityDialog::on_tabWidget_currentChanged(int index) {
    ui->webView->setVisible(index == -1 || ui->tabWidget->tabText(index) == "Structure");
}

void WaterQualityDialog::addJSObject() {
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("waterQualityDialog", this);
}

void WaterQualityDialog::toggleItem(const QString &itemName, const bool &checked) {
    QTreeWidgetItemIterator it(ui->trwStructure, QTreeWidgetItemIterator::All);
    
    ui->trwStructure->blockSignals(true);
    while (*it) {
        if ((*it)->data(1, Qt::UserRole).toString() == itemName) {
            (*it)->setCheckState(0, checked ? Qt::Checked : Qt::Unchecked);
            break;
        }
        it++;
    }
    ui->trwStructure->blockSignals(false);
}

void WaterQualityDialog::onTabularInputButtonClicked() {
	QToolButton *button = static_cast<QToolButton*>(sender());
    WaterQualityParameter *parameter = waterQualityRepository->findParameterByName(button->objectName(), WaterQualityParameterSection::PARAMETER);
    QList<int> groupsDistribuition;
    
    for (QString groupName : parameter->getGroups()) {
        WaterQualityParameter *groupParameter = waterQualityRepository->findParameterByName(groupName, WaterQualityParameterSection::PARAMETER);
        int groupValue = ui->trwParameter->findChild<QLineEdit*>(groupName)->text().toInt();
        
        if (!groupParameter->isInRange(groupValue)) {
            QMessageBox::warning(this, "Water Quality", QString("Group %1 must be between %2 and %3.").arg(groupParameter->getLabel()).arg(groupParameter->getRangeMinimum()).arg(groupParameter->getRangeMaximum()));
            return;
        }

        groupsDistribuition << groupValue;
    }

    WaterQualityParameterDialog dialog(this, parameter, groupsDistribuition);
    dialog.exec();
}