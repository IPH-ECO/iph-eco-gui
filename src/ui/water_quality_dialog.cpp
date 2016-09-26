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
#include <QTableWidgetItem>

WaterQualityDialog::WaterQualityDialog(QWidget *parent) :
	QDialog(parent),
    ui(new Ui::WaterQualityDialog),
    unsavedConfiguration(new WaterQualityConfiguration),
    currentConfiguration(unsavedConfiguration),
    waterQualityRepository(WaterQualityRepository::getInstance()),
    isWebViewLoaded(false)
{
    ui->setupUi(this);
    ui->trwParameter->header()->setStretchLastSection(false);
    ui->trwParameter->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->trwStructure->header()->setStretchLastSection(false);
    ui->trwStructure->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->trwInitialConditions->header()->setStretchLastSection(false);
    ui->trwInitialConditions->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    
    QObject::connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(addJSObject()));
    
    ui->webView->load(QUrl("qrc:/data/water_quality_diagram.html"));

    Project *project = IPHApplication::getCurrentProject();
    
    ui->cbxConfiguration->blockSignals(true);
    for (WaterQualityConfiguration *configuration : project->getWaterQualityConfigurations()) {
        ui->cbxConfiguration->addItem(configuration->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxConfiguration->blockSignals(false);

    ui->cbxHydrodynamicConfiguration->blockSignals(true);
    for (HydrodynamicConfiguration *configuration : project->getHydrodynamicConfigurations()) {
        ui->cbxHydrodynamicConfiguration->addItem(configuration->getName());
    }
    ui->cbxHydrodynamicConfiguration->setCurrentIndex(-1);
    ui->cbxHydrodynamicConfiguration->blockSignals(false);
}

void WaterQualityDialog::on_webView_loadFinished(bool ok) {
    waterQualityRepository->loadParameters(currentConfiguration);
    
    for (WaterQualityParameter *rootParameter : currentConfiguration->getRootParameters(WaterQualityParameterSection::PARAMETER)) {
        this->buildTreeWidgets(rootParameter);
    }
    
    for (WaterQualityParameter *rootParameter : currentConfiguration->getRootParameters(WaterQualityParameterSection::STRUCTURE)) {
        this->buildTreeWidgets(rootParameter);
    }
    
    for (WaterQualityParameter *rootParameter : currentConfiguration->getRootParameters(WaterQualityParameterSection::INITIAL_CONDITION)) {
        this->buildTreeWidgets(rootParameter);
    }
    
    this->bindCurrentConfigurationToTreeWidgets();
    
    isWebViewLoaded = true;
}

WaterQualityDialog::~WaterQualityDialog() {
    delete unsavedConfiguration;
	delete ui;
}

void WaterQualityDialog::bindCurrentConfigurationToTreeWidgets() {
    for (WaterQualityParameter *parameter : currentConfiguration->getParameters()) {
        QTreeWidget *treeWidget = nullptr;
        
        if (parameter->getSection() == WaterQualityParameterSection::STRUCTURE) {
            treeWidget = ui->trwStructure;
        } else if (parameter->getSection() == WaterQualityParameterSection::PARAMETER) {
            treeWidget = ui->trwParameter;
        } else {
            treeWidget = ui->trwInitialConditions;
        }
        
        QList<QTreeWidgetItem*> widgetItems = treeWidget->findItems(parameter->getLabel(), Qt::MatchExactly | Qt::MatchRecursive);
        
        for (QTreeWidgetItem *widgetItem : widgetItems) {
            if (widgetItem->data(0, Qt::UserRole).toString() == parameter->getName()) {
                parameter->setItemWidget(widgetItem);
            }
        }
        
        if (parameter->isCheckable()) {
            parameter->getItemWidget()->setCheckState(0, parameter->isChecked() ? Qt::Checked : Qt::Unchecked);
        } else if (parameter->getInputType() == WaterQualityParameterInputType::INLINE) {
            QTreeWidget *treeWidget = parameter->getSection() == WaterQualityParameterSection::PARAMETER ? ui->trwParameter : ui->trwInitialConditions;
            QLineEdit *lineEdit = treeWidget->findChild<QLineEdit*>(parameter->getName());
            lineEdit->setText(QString::number(parameter->getValue()));
        }
    }
}

void WaterQualityDialog::buildTreeWidgets(WaterQualityParameter *parameter) {
    QTreeWidget *treeWidget = nullptr;
    QTreeWidgetItem *parameterItem = nullptr;
    
    if (parameter->getSection() == WaterQualityParameterSection::STRUCTURE) {
        treeWidget = ui->trwStructure;
    } else if (parameter->getSection() == WaterQualityParameterSection::PARAMETER) {
        treeWidget = ui->trwParameter;
    } else {
        treeWidget = ui->trwInitialConditions;
    }
    
    if (parameter->getParent()) {
        parameterItem = new QTreeWidgetItem(parameter->getParent()->getItemWidget(), QStringList(parameter->getLabel()));
    } else {
        parameterItem = new QTreeWidgetItem(treeWidget, QStringList(parameter->getLabel()));
    }

    parameter->setItemWidget(parameterItem);
    
    treeWidget->blockSignals(true);
    if (parameter->getSection() == WaterQualityParameterSection::STRUCTURE) {
        parameterItem->setDisabled(!parameter->isEnabled());
    } else {
        if (parameter->getSection() == WaterQualityParameterSection::PARAMETER) {
            parameterItem->setHidden(!parameter->isEnabled());
        }
    }
    parameterItem->setToolTip(0, parameter->getDescription());
    parameterItem->setData(0, Qt::UserRole, QVariant(parameter->getName()));
    parameterItem->setData(1, Qt::UserRole, QVariant(parameter->getDiagramItem()));
    treeWidget->blockSignals(false);
    
    if (parameter->getInputType() == WaterQualityParameterInputType::INLINE) {
        QLineEdit *lineEdit = new QLineEdit(treeWidget);
        
        lineEdit->setAlignment(Qt::AlignRight);
        lineEdit->setEnabled(parameter->isEnabled());
        lineEdit->setObjectName(parameter->getName());
        
        treeWidget->setItemWidget(parameterItem, 1, lineEdit);
    } else if (parameter->getInputType() == WaterQualityParameterInputType::TABULAR) {
        QToolButton *toolButton = new QToolButton(treeWidget);
        
        toolButton->setObjectName(QString("%1-%2").arg(parameter->getName()).arg((int) parameter->getSection()));
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
        buildTreeWidgets(child);
    }
}

void WaterQualityDialog::loadFoodMatrix() {
    QStringList predatorsLabels;
    QStringList preysLabels;
    QList<FoodMatrixElement*> preys;
    QList<FoodMatrixElement*> predators;
    
    ui->tblFoodMatrix->clear();
    
    for (FoodMatrixElement *prey : waterQualityRepository->getPreys()) {
        WaterQualityParameter *parameter = prey->getParameter();
        QTreeWidgetItem *preyItem = parameter->getItemWidget();
        
        if (preyItem->isHidden()) {
            continue;
        }
        
        bool skipThisPrey = true;
        
        for (FoodMatrixElement *predator : prey->getPredators()) {
            QTreeWidgetItem *predatorItem = predator->getParameter()->getItemWidget();
            
            if (!predatorItem->isHidden()) {
                skipThisPrey = false;
                break;
            }
        }
        
        if (skipThisPrey) {
            continue;
        }
        
        if (prey->getGroup()) {
            int groupValue = ui->trwParameter->findChild<QLineEdit*>(prey->getGroup()->getName())->text().toInt();
            
            for (int i = 0; i < groupValue; i++) {
                preysLabels.append(QString("%1 %2").arg(prey->getLabel()).arg(i + 1));
                preys.append(prey);
            }
        } else {
            preysLabels.append(prey->getLabel());
            preys.append(prey);
        }
    }
    
    for (FoodMatrixElement *predator : waterQualityRepository->getPredators()) {
        QTreeWidgetItem *predatorItem = predator->getParameter()->getItemWidget();
        
        if (predatorItem->isHidden()) {
            continue;
        }
        
        if (predator->getGroup()) {
            int groupValue = static_cast<QLineEdit*>(ui->trwParameter->itemWidget(predator->getGroup()->getItemWidget(), 1))->text().toInt();
            
            for (int i = 0; i < groupValue; i++) {
                predatorsLabels.append(QString("%1 %2").arg(predator->getLabel()).arg(i + 1));
                predators.append(predator);
            }
        } else {
            predatorsLabels.append(predator->getLabel());
            predators.append(predator);
        }
    }
    
    if (preys.size() > 0 && predators.size() > 0) {
        ui->tblFoodMatrix->setColumnCount(preys.size());
        ui->tblFoodMatrix->setRowCount(predators.size());
        ui->tblFoodMatrix->setHorizontalHeaderLabels(preysLabels);
        ui->tblFoodMatrix->setVerticalHeaderLabels(predatorsLabels);
        ui->tblFoodMatrix->resizeColumnsToContents();
        
        for (int i = 0; i < predators.size(); i++) {
            for (int j = 0; j < preys.size(); j++) {
                QTableWidgetItem *tableItem = new QTableWidgetItem();
                
                if (predators[i]->getPreys().contains(preys[j])) {
                    double value = currentConfiguration->getFoodMatrixValue(predators[i]->getName(), preys[j]->getName());
                    
                    if (value == std::numeric_limits<double>::max()) {
                        value = waterQualityRepository->getFoodMatrixValue(predators[i], preys[j]);
                    }
                    
                    tableItem->setText(QString::number(value));
                } else {
                    tableItem->setFlags(Qt::NoItemFlags);
                    tableItem->setBackgroundColor(QColor(Qt::lightGray));
                }
                
                tableItem->setData(Qt::UserRole, QString(predators[i]->getName()).append("-").append(preys[j]->getName()));
                ui->tblFoodMatrix->setItem(i, j, tableItem);
            }
        }
    } else {
        ui->tblFoodMatrix->setColumnCount(0);
        ui->tblFoodMatrix->setRowCount(0);
    }
}

void WaterQualityDialog::loadInitialConditions() {
    for (WaterQualityParameter *parameter : currentConfiguration->getParameters()) {
        if (parameter->getSection() == WaterQualityParameterSection::INITIAL_CONDITION && parameter->getTarget()) {
            bool hidden;
            
            if (parameter->getTarget()->getSection() == WaterQualityParameterSection::STRUCTURE) {
                hidden = !parameter->getTarget()->isChecked();
            } else {
                hidden = parameter->getTarget()->getItemWidget()->isHidden();
            }
            
            parameter->getItemWidget()->setHidden(hidden);
        }
    }
}

void WaterQualityDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    if (!configurationName.isEmpty()) {
        currentConfiguration = IPHApplication::getCurrentProject()->getWaterQualityConfiguration(configurationName);
        waterQualityRepository->loadParameters(currentConfiguration);
        ui->edtConfigurationName->setText(currentConfiguration->getName());
        ui->cbxHydrodynamicConfiguration->setCurrentText(currentConfiguration->getHydrodynamicConfiguration()->getName());
    }
    
    this->bindCurrentConfigurationToTreeWidgets();
    this->loadFoodMatrix();
    this->loadInitialConditions();
}

void WaterQualityDialog::on_cbxHydrodynamicConfiguration_currentIndexChanged(const QString &hydrodynamicConfigurationName) {
    if (hydrodynamicConfigurationName.isEmpty()) {
        return;
    }
    
    HydrodynamicConfiguration *hydrodynamicConfiguration = IPHApplication::getCurrentProject()->getHydrodynamicConfiguration(hydrodynamicConfigurationName);
    currentConfiguration->setHydrodynamicConfiguration(hydrodynamicConfiguration);
}

void WaterQualityDialog::on_btnRemoveConfiguration_clicked() {
    QMessageBox::StandardButton question = QMessageBox::question(this, tr("Water Quality"), tr("Are you sure you want to remove the selected configuration?"));
    
    if (question == QMessageBox::Yes) {
        ui->cbxConfiguration->removeItem(ui->cbxConfiguration->currentIndex());
        IPHApplication::getCurrentProject()->removeWaterQualityConfiguration(ui->cbxConfiguration->currentText());
        this->on_btnNewConfiguration_clicked();
    }
}

void WaterQualityDialog::on_btnNewConfiguration_clicked() {
    currentConfiguration = unsavedConfiguration;
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxHydrodynamicConfiguration->setCurrentIndex(-1);
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
    
    if (ui->cbxHydrodynamicConfiguration->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Water Quality"), tr("Hydrodynamic configuration can't be empty."));
        return;
    }
    
    currentConfiguration->setName(newConfigurationName);
    
    for (WaterQualityParameter *parameter : currentConfiguration->getParameters(true)) {
        if (parameter->getInputType() == WaterQualityParameterInputType::INLINE) {
            QTreeWidget *treeWidget = parameter->getSection() == WaterQualityParameterSection::PARAMETER ? ui->trwParameter : ui->trwInitialConditions;
            QLineEdit *lineEdit = static_cast<QLineEdit*>(treeWidget->itemWidget(parameter->getItemWidget(), 1));
            parameter->setValue(lineEdit->text().toDouble());
        }
    }
    
    currentConfiguration->clearFoodMatrix();
    
    for (int i = 0; i < ui->tblFoodMatrix->rowCount(); i++) {
        for (int j = 0; j < ui->tblFoodMatrix->columnCount(); j++) {
            QTableWidgetItem *item = ui->tblFoodMatrix->item(i, j);
            QStringList predatorAndPrey = item->data(Qt::UserRole).toString().split("-");
            double value = item->text().toDouble();
            
            currentConfiguration->setFoodMatrixItem(predatorAndPrey.first(), predatorAndPrey.last(), value);
        }
    }

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
    
    waterQualityRepository->loadParameters(currentConfiguration);
}

void WaterQualityDialog::on_trwStructure_itemChanged(QTreeWidgetItem *item, int column) {
    WaterQualityParameter *sourceParameter = currentConfiguration->getParameter(item->data(0, Qt::UserRole).toString(), WaterQualityParameterSection::STRUCTURE);
    
    if (sourceParameter && (item->checkState(0) == Qt::Checked || item->checkState(0) == Qt::Unchecked)) {
        sourceParameter->setChecked(item->checkState(0) == Qt::Checked);
        sourceParameter->setValue(sourceParameter->isChecked());
        
        if (!sourceParameter->getDiagramItem().isEmpty()) {
            QString jsFunction = sourceParameter->isChecked() ? "enableVariable" : "disableVariable";
            ui->webView->page()->mainFrame()->evaluateJavaScript(jsFunction + "('" + sourceParameter->getDiagramItem() + "'); null");
        }
        
        if (sourceParameter->isChecked()) {
            WaterQualityParameter *parentParameter = sourceParameter->getParent();
            
            if (parentParameter) {
                if (parentParameter->isCheckable()) {
                    QTreeWidgetItem *parentItem = parentParameter->getItemWidget();
                    
                    if (parentItem->checkState(0) == Qt::Unchecked) {
                        parentItem->setCheckState(0, Qt::Checked);
                    }
                }
                
                if (isWebViewLoaded && parentParameter->isRadio()) {
                    for (WaterQualityParameter *child : parentParameter->getChildren()) {
                        if (child != sourceParameter) {
                            QTreeWidgetItem *childItem = child->getItemWidget();
                            
                            if (childItem) {
                                childItem->setCheckState(0, Qt::Unchecked);
                            }
                        }
                    }
                }
            }
        } else {
            for (WaterQualityParameter *child : sourceParameter->getChildren()) {
                QTreeWidgetItem *childItem = child->getItemWidget();
                
                if (childItem && childItem->checkState(0) == Qt::Checked) {
                    childItem->setCheckState(0, Qt::Unchecked);
                }
            }
            
            if (isWebViewLoaded && sourceParameter->isRadio()) {
                bool checkSibling = true;
                
                for (WaterQualityParameter *sibling : sourceParameter->getSiblings()) {
                    if (sibling->getItemWidget()->checkState(0) == Qt::Checked) {
                        checkSibling = false;
                    }
                }
                
                if (checkSibling) {
                    item->setCheckState(0, Qt::Checked);
                }
            }
        }
        
        if (sourceParameter->getTarget() && sourceParameter->getTarget()->getItemWidget()) {
            if (sourceParameter->getTarget()->isRadio()) {
                if (sourceParameter->isChecked()) {
                    for (WaterQualityParameter *sibling : sourceParameter->getTarget()->getSiblings()) {
                        if (sibling->isRadio()) {
                            sibling->getItemWidget()->setHidden(true);
                        }
                    }
                } else {
                    if (sourceParameter->getParent() && sourceParameter->getParent()->isChecked()) {
                        WaterQualityParameter *sourceParentParameter = sourceParameter->getParent()->getTarget();
                        
                        sourceParentParameter->getItemWidget()->setHidden(false);
                    }
                }
            }
            
            sourceParameter->getTarget()->getItemWidget()->setHidden(!sourceParameter->isChecked());
        }
    }
}

void WaterQualityDialog::on_tabWidget_currentChanged(int index) {
    ui->webView->setVisible(index == -1 || ui->tabWidget->tabText(index) == "Structure");
    
    if (ui->tabWidget->tabText(index) == "Food Matrix") {
        this->loadFoodMatrix();
    } else if (ui->tabWidget->tabText(index) == "Initial Conditions") {
        this->loadInitialConditions();
    }
}

void WaterQualityDialog::addJSObject() {
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("waterQualityDialog", this);
}

void WaterQualityDialog::toggleItem(const QString &itemName, const bool &checked) {
    if (!isWebViewLoaded) {
        return;
    }
    
    WaterQualityParameter *parameter = currentConfiguration->getParameterByDiagramItem(itemName);
    
    if (parameter) {
        parameter->setChecked(checked);
        parameter->getItemWidget()->setCheckState(0, parameter->isChecked() ? Qt::Checked : Qt::Unchecked);
    }
}

void WaterQualityDialog::onTabularInputButtonClicked() {
	QStringList parameterNameAndSection = static_cast<QToolButton*>(sender())->objectName().split("-");
    WaterQualityParameter *parameter = currentConfiguration->getParameter(parameterNameAndSection.first(), (WaterQualityParameterSection) parameterNameAndSection.last().toInt());
    QList<int> groupsDistribuition;
    
    for (QString groupName : parameter->getGroups()) {
        WaterQualityParameter *groupParameter = currentConfiguration->getParameter(groupName, WaterQualityParameterSection::PARAMETER);
        int groupValue = ui->trwParameter->findChild<QLineEdit*>(groupName)->text().toInt();
        
        if (!groupParameter->isInRange(groupValue)) {
            QMessageBox::warning(this, "Water Quality", QString("Group %1 must be between %2 and %3.").arg(groupParameter->getLabel()).arg(groupParameter->getRangeMinimum()).arg(groupParameter->getRangeMaximum()));
            return;
        }

        groupsDistribuition << groupValue;
    }

    WaterQualityParameterDialog dialog(this, currentConfiguration, parameter, groupsDistribuition);
    dialog.exec();
}

bool WaterQualityDialog::isItemWidgetVisible(QTreeWidgetItem *item) const {
    if (!item) {
        return false;
    }
    
    while (item) {
        if (item->isHidden()) {
            return false;
        }
        
        item = item->parent();
    }
    
    return true;
}