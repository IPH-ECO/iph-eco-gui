#include <ui/non_vertically_integrated_boundary_condition_dialog.h>
#include "ui_non_vertically_integrated_boundary_condition_dialog.h"

#include <ui/time_series_dialog.h>

#include <QMessageBox>
#include <vtkCellData.h>

NonVerticallyIntegratedBoundaryConditionDialog::NonVerticallyIntegratedBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition) :
    HydrodynamicBoundaryConditionDialog(configuration, boundaryCondition),
    ui(new Ui::NonVerticallyIntegratedBoundaryConditionDialog)
{
    ui->setupUi(this);
    this->setupBaseUi();
    ui->tblRanges->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    btnIndividualObjectPicker->setEnabled(false);
    
    int i = 0;
    
    if (!isNewBoundaryCondition) {
        ui->edtName->setText(boundaryCondition->getName());
        ui->lblElementIds->setText(boundaryCondition->getObjectIdsStr());
        
        for (VerticallyIntegratedRange *range : currentBoundaryCondition->getVerticallyIntegratedRanges()) {
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
}

NonVerticallyIntegratedBoundaryConditionDialog::~NonVerticallyIntegratedBoundaryConditionDialog() {
    for (VerticallyIntegratedRange *unsavedRange : unsavedRanges) {
        delete unsavedRange;
    }
    
    delete ui;
}

void NonVerticallyIntegratedBoundaryConditionDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    currentBoundaryCondition->setName(ui->edtName->text());
    currentBoundaryCondition->setType(BoundaryConditionType::WATER_FLOW);
    currentBoundaryCondition->setObjectIds(ui->lblElementIds->text());
    currentBoundaryCondition->setInputModule(InputModule::HYDRODYNAMIC);
    currentBoundaryCondition->setVerticallyIntegrated(false);
    
    for (int i = 0; i < ui->tblRanges->rowCount(); i++) {
        QComboBox *cbxFunction = static_cast<QComboBox*>(ui->tblRanges->cellWidget(i, 2));
        VerticallyIntegratedRange *range = static_cast<VerticallyIntegratedRange*>(ui->tblRanges->verticalHeaderItem(i)->data(Qt::UserRole).value<void*>());
        
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
        
        currentBoundaryCondition->addVerticallyIntegratedRange(range);
        unsavedRanges.remove(range);
    }
    
    configuration->addBoundaryCondition(currentBoundaryCondition);
    emit boundaryConditionUpdated(currentBoundaryCondition);
    
    HydrodynamicBoundaryConditionDialog::accept();
}

bool NonVerticallyIntegratedBoundaryConditionDialog::isValid() {
    if (ui->edtName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Please input the boundary condition name."));
        return false;
    }
    
    if (currentBoundaryCondition->getObjectIds().isEmpty()) {
        QString message = tr("Please pick at least one %1 from the grid.").arg(ui->cbxType->currentText() == "Water Level" ? "cell" : "edge");
        QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), message);
        return false;
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
        
        GridDataConfiguration *gridDataConfiguration = configuration->getGridDataConfiguration();
        Mesh *mesh = gridDataConfiguration->getMesh();
        QSet<vtkIdType> boundaryCells = mesh->getBoundaryCellIds(currentBoundaryCondition->getVTKObjectIds());
        GridData *bathymetryGridData = nullptr;
        
        for (GridData *gridData : gridDataConfiguration->getGridDataVector()) {
            if (gridData->getGridDataType() == GridDataType::BATHYMETRY) {
                bathymetryGridData = gridData;
                break;
            }
        }
        
        double minimumWeight = std::numeric_limits<double>::min();
        
        for (vtkIdType cellId : boundaryCells) {
            QByteArray arrayName = bathymetryGridData->getName().toLocal8Bit();
            double weight = mesh->getMeshPolyData()->GetCellData()->GetScalars(arrayName.constData())->GetTuple1(cellId);
            
            if (minimumWeight == std::numeric_limits<double>::min()) {
                minimumWeight = weight;
            } else if (weight < minimumWeight) {
                minimumWeight = weight;
            }
        }
        
        if (minimumElevationStr.toDouble() < minimumWeight) {
            QMessageBox::warning(this, tr("Hydrodynamic Boundary Condition"), tr("Minimum elevation must be greater than or equal to %1 (line %2).").arg(minimumWeight).arg(i + 1));
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

void NonVerticallyIntegratedBoundaryConditionDialog::on_btnAddRange_clicked() {
    int rowCount = ui->tblRanges->rowCount();
    VerticallyIntegratedRange *range = new VerticallyIntegratedRange();
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

void NonVerticallyIntegratedBoundaryConditionDialog::on_btnRemoveRange_clicked() {
    int currentRow = ui->tblRanges->currentRow();
    
    if (currentRow > -1 && QMessageBox::question(this, "Vertical Integrated Boundary Condition", "Are you sure?") == QMessageBox::Yes) {
        ui->tblRanges->removeRow(currentRow);
    }
}

void NonVerticallyIntegratedBoundaryConditionDialog::onCbxFunctionCurrentTextChanged(const QString &text) {
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

void NonVerticallyIntegratedBoundaryConditionDialog::onBtnTimeSeriesClicked() {
    QList<TimeSeries*> *timeSeriesList = static_cast<QList<TimeSeries*>*>(sender()->property("timeSeries").value<void*>());
    TimeSeriesDialog *dialog = new TimeSeriesDialog(this, TimeSeriesType::DEFAULT);
    dialog->setObjectType(TimeSeriesObjectType::vertically_integrated_range);
    dialog->loadTimeSeriesList(timeSeriesList);
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        sender()->setProperty("timeSeriesChanged", dialog->hasChanges());
    }
}
