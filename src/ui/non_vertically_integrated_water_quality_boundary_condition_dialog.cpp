#include <ui/non_vertically_integrated_water_quality_boundary_condition_dialog.h>
#include "ui_non_vertically_integrated_water_quality_boundary_condition_dialog.h"

#include <ui/water_quality_dialog.h>
#include <ui/time_series_dialog.h>
#include <repository/water_quality_repository.h>
#include "ui_water_quality_dialog.h"

#include <QMessageBox>

NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::NonVerticallyIntegratedWaterQualityBoundaryConditionDialog(QDialog *parent, WaterQualityConfiguration *configuration, WaterQualityBoundaryCondition *boundaryCondition) :
	QDialog(parent),
	ui(new Ui::NonVerticallyIntegratedWaterQualityBoundaryConditionDialog),
	currentConfiguration(configuration),
	currentBoundaryCondition(boundaryCondition),
	isNewBoundaryCondition(boundaryCondition == nullptr)
{
	ui->setupUi(this);
    ui->tblRanges->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    WaterQualityRepository *repository = WaterQualityRepository::getInstance();
    
    for (HydrodynamicBoundaryCondition *boundaryCondition : configuration->getHydrodynamicConfiguration()->getBoundaryConditions()) {
        ui->cbxHydroBoundaryCondition->addItem(boundaryCondition->getName());
    }
    
    for (WaterQualityParameter *variable : repository->getBoundaryConditionVariables()) {
        WaterQualityParameter *target = variable->getTarget();
        
        if (target) {
            if (target->getItemWidget()->checkState(0) == Qt::Checked) {
                if (variable->isGroup()) {
                    QString groupParameterName = variable->getName() + "Groups";
                    WaterQualityParameter *groupParameter = currentConfiguration->getParameter(groupParameterName, WaterQualityParameterSection::PARAMETER);
                    
                    for (WaterQualityParameter *group : groupParameter->getChildren()) {
                        for (int j = 1; j <= group->getValue(); j++) {
                            ui->cbxVariable->addItem(QString("%1 - %2 %3").arg(variable->getLabel()).arg(group->getLabel()).arg(j));
                        }
                    }
                } else if (variable->isRadio()) {
                    ui->cbxVariable->addItem(variable->getLabel(), qVariantFromValue((void*) variable));
                } else {
                    ui->cbxVariable->addItem(variable->getLabel());
                }
            }
        } else {
            ui->cbxVariable->addItem(variable->getLabel());
        }
    }
    
    int count = ui->cbxVariable->count();
    
    for (int i = 0; i < count; i++) {
        WaterQualityParameter *variable = static_cast<WaterQualityParameter*>(ui->cbxVariable->itemData(i).value<void*>());
        
        if (!variable) {
            continue;
        }
        
        WaterQualityParameter *targetParameter = variable->getTarget();
        
        if (targetParameter->isChecked()) {
            for (WaterQualityParameter *child : targetParameter->getChildren()) {
                if (child->isChecked()) {
                    ui->cbxVariable->removeItem(i);
                    count--;
                    break;
                }
            }
        }
    }
    
    int i = 0;
    
    if (isNewBoundaryCondition) {
        this->currentBoundaryCondition = new WaterQualityBoundaryCondition();
    } else {
        for (NonVerticallyIntegratedRange *range : currentBoundaryCondition->getNonVerticallyIntegratedRanges()) {
            QTableWidgetItem *rangeItem = new QTableWidgetItem();
            
            rangeItem->setData(Qt::UserRole, qVariantFromValue((void*) range));
            
            ui->tblRanges->insertRow(i);
            ui->tblRanges->setVerticalHeaderItem(i, rangeItem);
            ui->tblRanges->setItem(i, 0, new QTableWidgetItem(QString::number(range->getMinimumElevation())));
            ui->tblRanges->setItem(i, 1, new QTableWidgetItem(QString::number(range->getMaximumElevation())));
            
            QToolButton *btnTimeSeries = new QToolButton();
            btnTimeSeries->setText("...");
            btnTimeSeries->setProperty("timeSeries", qVariantFromValue((void*) range->getTimeSeriesListPointer()));
            connect(btnTimeSeries, SIGNAL(clicked()), this, SLOT(onBtnTimeSeriesClicked()));
            
            QLineEdit *edtConstant = new QLineEdit();
            QWidget *widget = new QWidget();
            QHBoxLayout *layout = new QHBoxLayout(widget);
            
            layout->addWidget(edtConstant);
            layout->addWidget(btnTimeSeries);
            layout->setAlignment(Qt::AlignCenter);
            layout->setContentsMargins(0, 0, 0, 0);
            
            widget->setLayout(layout);
            
            QComboBox *cbxFunction = new QComboBox(ui->tblRanges);
            
            cbxFunction->addItem("Constant");
            cbxFunction->addItem("Time Series");
            cbxFunction->setProperty("rowNumber", i);
            cbxFunction->setItemData(0, qVariantFromValue(edtConstant));
            cbxFunction->setItemData(1, qVariantFromValue(btnTimeSeries));
            ui->tblRanges->setCellWidget(i, 2, cbxFunction);
            ui->tblRanges->setCellWidget(i, 3, widget);
            
            if (range->getFunction() == BoundaryConditionFunction::CONSTANT) {
                cbxFunction->setCurrentText("Constant");
                edtConstant->setText(QString::number(range->getValue()));
                btnTimeSeries->hide();
            } else {
                cbxFunction->setCurrentText("Time Series");
                edtConstant->hide();
            }
            
            connect(cbxFunction, SIGNAL(currentTextChanged(const QString&)), this, SLOT(onCbxFunctionCurrentTextChanged(const QString&)));
            i++;
        }
    }
    
    if (currentBoundaryCondition->getHydrodynamicBoundaryCondition()) {
        ui->cbxHydroBoundaryCondition->setCurrentText(currentBoundaryCondition->getHydrodynamicBoundaryCondition()->getName());
    }
}

NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::~NonVerticallyIntegratedWaterQualityBoundaryConditionDialog() {
    for (NonVerticallyIntegratedRange *unsavedRange : unsavedRanges) {
        delete unsavedRange;
    }
    
    delete ui;
}

void NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    HydrodynamicConfiguration *hydrodynamicConfiguration = currentConfiguration->getHydrodynamicConfiguration();
    HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition = hydrodynamicConfiguration->getBoundaryCondition(ui->cbxHydroBoundaryCondition->currentText());
    
    currentBoundaryCondition->setName(ui->cbxVariable->currentText());
    currentBoundaryCondition->setHydrodynamicBoundaryCondition(hydrodynamicBoundaryCondition);
    currentBoundaryCondition->setInputModule(InputModule::WATER_QUALITY);
    currentBoundaryCondition->setVerticallyIntegrated(false);
    
    for (int i = 0; i < ui->tblRanges->rowCount(); i++) {
        QComboBox *cbxFunction = static_cast<QComboBox*>(ui->tblRanges->cellWidget(i, 2));
        NonVerticallyIntegratedRange *range = static_cast<NonVerticallyIntegratedRange*>(ui->tblRanges->verticalHeaderItem(i)->data(Qt::UserRole).value<void*>());
        
        range->setMinimumElevation(ui->tblRanges->item(i, 0)->text().toDouble());
        range->setMaximumElevation(ui->tblRanges->item(i, 1)->text().toDouble());
        
        if (cbxFunction->currentText() == "Constant") {
            QLineEdit *edtConstant = cbxFunction->itemData(0).value<QLineEdit*>();
            
            range->setFunction(BoundaryConditionFunction::CONSTANT);
            range->setValue(edtConstant->text().toDouble());
        } else {
            QToolButton *btnTimeSeries = cbxFunction->itemData(1).value<QToolButton*>();
            
            range->setTimeSeriesChanged(btnTimeSeries->property("timeSeriesChanged").toBool());
            range->setFunction(BoundaryConditionFunction::TIME_SERIES);
        }
        
        currentBoundaryCondition->addNonVerticallyIntegratedRange(range);
        unsavedRanges.remove(range);
    }
    
    currentConfiguration->addBoundaryCondition(currentBoundaryCondition);
    emit boundaryConditionUpdated(currentBoundaryCondition);
    
    QDialog::accept();
}

void NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::reject() {
    undoChanges();
    QDialog::reject();
}

bool NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::isValid() {
    if (ui->cbxHydroBoundaryCondition->currentIndex() == - 1) {
        QMessageBox::warning(this, tr("Water Quality Boundary Condition"), tr("Hydrodynamic boundary condition can't be empty"));
        return false;
    }
    
    if (ui->cbxVariable->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Water Quality Boundary Condition"), tr("Variable can't be empty"));
        return false;
    }
    
    HydrodynamicConfiguration *hydrodynamicConfiguration = currentConfiguration->getHydrodynamicConfiguration();
    HydrodynamicBoundaryCondition *hydrodynamicBoundaryCondition = hydrodynamicConfiguration->getBoundaryCondition(ui->cbxHydroBoundaryCondition->currentText());
    
    for (WaterQualityBoundaryCondition *existentBoundaryCondition : currentConfiguration->getBoundaryConditions()) {
        if (existentBoundaryCondition->getHydrodynamicBoundaryCondition() == hydrodynamicBoundaryCondition && existentBoundaryCondition->getName() == ui->cbxVariable->currentText()) {
            QMessageBox::warning(this, tr("Water Quality Boundary Condition"), QString("A boundary condition using variable %1 already exists.").arg(ui->cbxVariable->currentText()));
            return false;
        }
    }
    
    if (ui->tblRanges->rowCount() == 0) {
        QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), "At least one range must be defined.");
        return false;
    }
    
    for (int i = 0; i < ui->tblRanges->rowCount(); i++) {
        QString minimumElevationStr = ui->tblRanges->item(i, 0)->text();
        QString maximumElevationStr = ui->tblRanges->item(i, 1)->text();
        
        if (minimumElevationStr.isEmpty() || maximumElevationStr.isEmpty()) {
            QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), QString("Elevation interval can't be blank (line %1).").arg(i + 1));
            return false;
        } else if (minimumElevationStr.toDouble() > maximumElevationStr.toDouble()) {
            QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), QString("Invalid elevation interval (line %1).").arg(i + 1));
            return false;
        }
        
        QComboBox *cbxFunction = static_cast<QComboBox*>(ui->tblRanges->cellWidget(i, 2));
        QLineEdit *edtConstant = cbxFunction->itemData(0).value<QLineEdit*>();
        
        if (cbxFunction->currentText() == "Constant" && edtConstant->text().isEmpty()) {
            QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), QString("Constant can't be empty (line %1).").arg(i + 1));
            return false;
        }
        
        if (cbxFunction->currentText() == "Time Series") {
            QToolButton *btnTimeSeries = cbxFunction->itemData(1).value<QToolButton*>();
            QList<TimeSeries*> *timeSeriesListPointer = static_cast<QList<TimeSeries*>*>(btnTimeSeries->property("timeSeries").value<void*>());
            
            if (!timeSeriesListPointer || timeSeriesListPointer->isEmpty()) {
                QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), QString("Time series can't be empty (line %1).").arg(i + 1));
                return false;
            }
        }
    }
    
    return true;
}

void NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::undoChanges() {
    
}

void NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::on_btnAddRange_clicked() {
    int rowCount = ui->tblRanges->rowCount();
    NonVerticallyIntegratedRange *range = new NonVerticallyIntegratedRange();
    QTableWidgetItem *rangeItem = new QTableWidgetItem();
    
    rangeItem->setData(Qt::UserRole, qVariantFromValue((void*) range));
    unsavedRanges.insert(range);
    
    ui->tblRanges->insertRow(rowCount);
    ui->tblRanges->setVerticalHeaderItem(rowCount, rangeItem);
    ui->tblRanges->setItem(rowCount, 0, new QTableWidgetItem(""));
    ui->tblRanges->setItem(rowCount, 1, new QTableWidgetItem(""));
    
    QToolButton *btnTimeSeries = new QToolButton();
    btnTimeSeries->setText("...");
    btnTimeSeries->setProperty("timeSeries", qVariantFromValue((void*) range->getTimeSeriesListPointer()));
    btnTimeSeries->hide();
    connect(btnTimeSeries, SIGNAL(clicked()), this, SLOT(onBtnTimeSeriesClicked()));
    
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    QLineEdit *edtConstant = new QLineEdit();
    
    layout->addWidget(edtConstant);
    layout->addWidget(btnTimeSeries);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0, 0, 0, 0);
    widget->setLayout(layout);
    ui->tblRanges->setCellWidget(rowCount, 3, widget);
    
    QComboBox *cbxFunction = new QComboBox(ui->tblRanges);
    cbxFunction->addItem("Constant");
    cbxFunction->addItem("Time Series");
    cbxFunction->setProperty("rowNumber", rowCount);
    cbxFunction->setItemData(0, qVariantFromValue(edtConstant));
    cbxFunction->setItemData(1, qVariantFromValue(btnTimeSeries));
    connect(cbxFunction, SIGNAL(currentTextChanged(const QString&)), this, SLOT(onCbxFunctionCurrentTextChanged(const QString&)));
    ui->tblRanges->setCellWidget(rowCount, 2, cbxFunction);
}

void NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::on_btnRemoveRange_clicked() {
    int currentRow = ui->tblRanges->currentRow();
    
    if (currentRow > -1 && QMessageBox::question(this, "Vertical Integrated Boundary Condition", "Are you sure?") == QMessageBox::Yes) {
        ui->tblRanges->removeRow(currentRow);
    }
}

void NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::onCbxFunctionCurrentTextChanged(const QString &text) {
    QComboBox *cbxFunction = static_cast<QComboBox*>(sender());
    QLineEdit *edtConstant = cbxFunction->itemData(0).value<QLineEdit*>();
    QToolButton *btnTimeSeries = cbxFunction->itemData(1).value<QToolButton*>();
    
    if (text == "Constant") {
        btnTimeSeries->hide();
        edtConstant->show();
    } else {
        edtConstant->hide();
        btnTimeSeries->show();
    }
}

void NonVerticallyIntegratedWaterQualityBoundaryConditionDialog::onBtnTimeSeriesClicked() {
    QList<TimeSeries*> *timeSeriesList = static_cast<QList<TimeSeries*>*>(sender()->property("timeSeries").value<void*>());
    TimeSeriesDialog *dialog = new TimeSeriesDialog(this, TimeSeriesType::DEFAULT);
    dialog->setObjectType(TimeSeriesObjectType::VERTICAL_INTEGRATED_RANGE);
    dialog->loadTimeSeriesList(timeSeriesList);
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        sender()->setProperty("timeSeriesChanged", dialog->hasChanges());
    }
}
