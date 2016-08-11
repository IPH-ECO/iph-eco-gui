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
    for (WaterQualityParameter *rootParameter : waterQualityRepository->getRootParameters(WaterQualityParameterSection::PARAMETER)) {
        this->buildParameterTree(rootParameter);
    }
    
    for (WaterQualityParameter *rootParameter : waterQualityRepository->getRootParameters(WaterQualityParameterSection::STRUCTURE)) {
        this->buildParameterTree(rootParameter);
    }
    
    isWebViewLoaded = true;
}

WaterQualityDialog::~WaterQualityDialog() {
    delete unsavedConfiguration;
	delete ui;
}

void WaterQualityDialog::buildParameterTree(WaterQualityParameter *parameter) {
    QTreeWidget *treeWidget = parameter->getSection() == WaterQualityParameterSection::STRUCTURE ? ui->trwStructure : ui->trwParameter;
    QTreeWidgetItem *parameterItem = nullptr;
    bool isDisabled = !parameter->isEnabled();
    
    if (parameter->getParent()) {
        parameterItem = new QTreeWidgetItem(parameter->getParent()->getItemWidget(), QStringList(parameter->getLabel()));
    } else {
        parameterItem = new QTreeWidgetItem(treeWidget, QStringList(parameter->getLabel()));
    }

    parameter->setItemWidget(parameterItem);
    
    treeWidget->blockSignals(true);
    if (parameter->getSection() == WaterQualityParameterSection::STRUCTURE) {
        parameterItem->setDisabled(isDisabled);
    } else {
        parameterItem->setHidden(isDisabled);
    }
    parameterItem->setToolTip(0, parameter->getDescription());
    parameterItem->setData(0, Qt::UserRole, QVariant(parameter->getName()));
    parameterItem->setData(1, Qt::UserRole, QVariant(parameter->getDiagramItem()));
    treeWidget->blockSignals(false);
    
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
        sourceParameter->setChecked(item->checkState(0) == Qt::Checked);
        
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
                            childItem->setCheckState(0, Qt::Unchecked);
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
        
        WaterQualityParameter *targetParameter = waterQualityRepository->findParameterByName(sourceParameter->getTarget(), WaterQualityParameterSection::PARAMETER);
        
        if (targetParameter && targetParameter->getItemWidget()) {
            if (targetParameter->isRadio()) {
                if (sourceParameter->isChecked()) {
                    for (WaterQualityParameter *sibling : targetParameter->getSiblings()) {
                        if (sibling->isRadio()) {
                            sibling->getItemWidget()->setHidden(true);
                        }
                    }
                } else {
                    if (sourceParameter->getParent() && sourceParameter->getParent()->isChecked()) {
                        WaterQualityParameter *sourceParentParameter = waterQualityRepository->findParameterByName(sourceParameter->getParent()->getTarget(), WaterQualityParameterSection::PARAMETER);
                        
                        sourceParentParameter->getItemWidget()->setHidden(false);
                    }
                }
            }
            
            targetParameter->getItemWidget()->setHidden(!sourceParameter->isChecked());
        }
    }
}

void WaterQualityDialog::on_tabWidget_currentChanged(int index) {
    ui->webView->setVisible(index == -1 || ui->tabWidget->tabText(index) == "Structure");
    
    if (ui->tabWidget->tabText(index) == "Food Matrix") {
        QStringList predatorsLabels;
        QStringList preysLabels;
        QList<FoodMatrixElement*> preys;
        QList<FoodMatrixElement*> predators;
        
        for (FoodMatrixElement *prey : waterQualityRepository->getPreys()) {
            WaterQualityParameter *parameter = prey->getParameter();
            
            if (parameter->isCheckable() && !parameter->isChecked()) {
                continue;
            }
            
            bool skipThisPrey = true;
            
            for (FoodMatrixElement *predator : prey->getPredators()) {
                if (predator->getParameter()->isCheckable() && predator->getParameter()->isChecked()) {
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
            WaterQualityParameter *parameter = predator->getParameter();
            
            if (parameter->isCheckable() && !parameter->isChecked()) {
                continue;
            }
            
            if (predator->getGroup()) {
                int groupValue = ui->trwParameter->findChild<QLineEdit*>(predator->getGroup()->getName())->text().toInt();
                
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
            
            for (int i = 0; i < predators.size(); i++) {
                FoodMatrixElement *predator = predators[i];
                
                for (int j = 0; j < preys.size(); j++) {
                    FoodMatrixElement *prey = preys[j];
                    QTableWidgetItem *tableItem = new QTableWidgetItem();
                    
                    if (predator->getPreys().contains(prey)) {
                        double value = waterQualityRepository->getFoodMatrixValue(predator, prey);
                        tableItem->setText(QString::number(value));
                    } else {
                        tableItem->setFlags(Qt::NoItemFlags);
                        tableItem->setBackgroundColor(QColor(Qt::lightGray));
                    }
                    
                    ui->tblFoodMatrix->setItem(i, j, tableItem);
                }
            }
        } else {
            ui->tblFoodMatrix->setColumnCount(0);
            ui->tblFoodMatrix->setRowCount(0);
        }
    }
}

void WaterQualityDialog::addJSObject() {
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("waterQualityDialog", this);
}

void WaterQualityDialog::toggleItem(const QString &itemName, const bool &checked) {
    if (!isWebViewLoaded) {
        return;
    }
    
    WaterQualityParameter *parameter = waterQualityRepository->findParameterByDiagramItem(itemName);
    parameter->setChecked(checked);
    parameter->getItemWidget()->setCheckState(0, parameter->isChecked() ? Qt::Checked : Qt::Unchecked);
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