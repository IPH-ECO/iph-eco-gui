#include <ui/hydrodynamic_boundary_condition_dialog.h>

#include <QIcon>
#include <QLabel>
#include <QComboBox>
#include <QStringList>
#include <QMessageBox>
#include <QColorDialog>
#include <QApplication>

#include <vtkCell.h>
#include <vtkProperty.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>

#include <ui/hydrodynamic_data_dialog.h>
#include <ui/hydrodynamic_vtk_widget.h>
#include <ui/time_series_dialog.h>

HydrodynamicBoundaryConditionDialog::HydrodynamicBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition) :
    configuration(configuration),
    currentBoundaryCondition(boundaryCondition),
    isNewBoundaryCondition(boundaryCondition == nullptr)
{
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (widget->objectName() == "MainWindow") {
            hydrodynamicDataDialog = static_cast<HydrodynamicDataDialog*>(widget->findChild<QWidget*>("HydrodynamicDataDialog"));
            break;
        }
    }
    
    if (isNewBoundaryCondition) {
        this->currentBoundaryCondition = new HydrodynamicBoundaryCondition();
    }
    
    HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
    vtkWidget->updateBoundaryCondition(currentBoundaryCondition);
}

void HydrodynamicBoundaryConditionDialog::setupBaseUi() {
    connect(currentBoundaryCondition, SIGNAL(objectIdsChanged()), this, SLOT(showObjectIds()));
    
    btnIndividualObjectPicker = new QToolButton(this);
    btnIndividualObjectPicker->setCheckable(true);
    btnIndividualObjectPicker->setEnabled(false);
	btnIndividualObjectPicker->setIcon(QIcon(":/icons/individual-object-picker.png"));
	btnIndividualObjectPicker->setToolTip("Individual cell picker");
    connect(btnIndividualObjectPicker, SIGNAL(clicked(bool)), this, SLOT(btnIndividualObjectPicker_clicked(bool)));

	btnMultipleObjectPicker = new QToolButton(this);
    btnMultipleObjectPicker->setCheckable(true);
	btnMultipleObjectPicker->setIcon(QIcon(":/icons/multiple-object-picker.png"));
	btnMultipleObjectPicker->setToolTip("Multiple object picker");
    connect(btnMultipleObjectPicker, SIGNAL(clicked(bool)), this, SLOT(btnMultipleObjectPicker_clicked(bool)));
    
    btnClearSelection = new QToolButton(this);
    btnClearSelection->setIcon(QIcon(":/icons/edit-clear-2.png"));
    btnClearSelection->setToolTip("Clear selection");
    connect(btnClearSelection, SIGNAL(clicked()), this, SLOT(btnClearSelection_clicked()));
    
    QToolButton *btnCellColor = this->findChild<QToolButton*>("btnCellColor");
    QColor color = QColor(this->currentBoundaryCondition->getCellColor());
    QPixmap px(10, 10);
    
    px.fill(color);
    btnCellColor->setIcon(px);
    
    QDialogButtonBox *buttonBox = this->findChild<QDialogButtonBox*>();
    buttonBox->addButton(btnIndividualObjectPicker, QDialogButtonBox::ActionRole);
    buttonBox->addButton(btnMultipleObjectPicker, QDialogButtonBox::ActionRole);
    buttonBox->addButton(btnClearSelection, QDialogButtonBox::ActionRole);
    buttonBox->setCenterButtons(true);
    
    this->originalObjectIds = this->currentBoundaryCondition->getObjectIds();
}

HydrodynamicBoundaryConditionDialog::~HydrodynamicBoundaryConditionDialog() {
    if (isNewBoundaryCondition) {
        delete currentBoundaryCondition;
    }
}

void HydrodynamicBoundaryConditionDialog::on_btnCellColor_clicked() {
    QColor color = QColorDialog::getColor(Qt::white, this, "Select a cell color");
    
    if (color.isValid()) {
        QToolButton *btnCellColor = this->findChild<QToolButton*>("btnCellColor");
        QPixmap px(10, 10);
        px.fill(color);
        
        currentBoundaryCondition->setCellColor(color.name());
        currentBoundaryCondition->getSelectionActor()->GetProperty()->SetColor(color.redF(), color.greenF(), color.blueF());
        btnCellColor->setIcon(px);
        
        HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
        vtkWidget->update();
    }
}

void HydrodynamicBoundaryConditionDialog::btnIndividualObjectPicker_clicked(bool checked) {
    HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
    
    vtkWidget->togglePicker(checked, PickerMode::EACH_CELL);
    vtkWidget->toggleZoomArea(false);
    hydrodynamicDataDialog->toggleZoomAreaAction(true);
    
    if (checked) {
        btnMultipleObjectPicker->blockSignals(true);
        btnMultipleObjectPicker->setChecked(false);
        btnMultipleObjectPicker->blockSignals(false);
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void HydrodynamicBoundaryConditionDialog::btnMultipleObjectPicker_clicked(bool checked) {
    HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
    QComboBox *cbxType = this->findChild<QComboBox*>("cbxType");
    
    // vtkWidget->togglePicker(checked, cbxType->currentText() == "Water Level" ? PickerMode::MULTIPLE_CELL : PickerMode::MULTIPLE_EDGE);
    vtkWidget->togglePicker(checked, PickerMode::MULTIPLE_EDGE);
    vtkWidget->toggleZoomArea(false);
    hydrodynamicDataDialog->toggleZoomAreaAction(checked);
    
    if (checked) {
        btnIndividualObjectPicker->blockSignals(true);
        btnIndividualObjectPicker->setChecked(false);
        btnIndividualObjectPicker->blockSignals(false);
        hydrodynamicDataDialog->activateWindow();
    } else {
        this->activateWindow();
    }
}

void HydrodynamicBoundaryConditionDialog::btnClearSelection_clicked() {
    QString question = tr("Are you sure you want to clear the current selection?");
    QMessageBox::StandardButton button = QMessageBox::question(this, tr("Hydrodynamic Boundary Condition"), question);
    
    if (button == QMessageBox::Yes) {
        HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
       
        this->findChild<QLabel*>("lblElementIds")->setText("-");
        vtkWidget->getMouseInteractor()->clearSelection();
    }
}

void HydrodynamicBoundaryConditionDialog::toggleLabelsActor(bool show) {
    currentBoundaryCondition->getLabelsActor()->SetVisibility(show);
}

void HydrodynamicBoundaryConditionDialog::showObjectIds() {
    QSet<vtkIdType> selectedCellsIds = currentBoundaryCondition->getObjectIds();
    QLabel *lblElementIds = this->findChild<QLabel*>("lblElementIds");
    
    if (selectedCellsIds.isEmpty()) {
        lblElementIds->setText("-");
    } else {
        QStringList cells;
        
        for (vtkIdType v : selectedCellsIds) {
            cells.append(QString::number(v));
        }
        
        lblElementIds->setText(cells.join(","));
    }
}

void HydrodynamicBoundaryConditionDialog::reject() {
    if (!isNewBoundaryCondition) {
        currentBoundaryCondition->setObjectIds(originalObjectIds);
    }
    
    HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
    vtkWidget->clearBoundaryCondition(currentBoundaryCondition);
    hydrodynamicDataDialog->toggleWidgets(true);
    
    QDialog::reject();
}

void HydrodynamicBoundaryConditionDialog::accept() {
    btnIndividualObjectPicker_clicked(false);
    btnMultipleObjectPicker_clicked(false);
    hydrodynamicDataDialog->toggleZoomAreaAction(true);
    hydrodynamicDataDialog->toggleWidgets(true);
    
    QDialog::accept();
}

void HydrodynamicBoundaryConditionDialog::closeEvent(QCloseEvent *event) {
    currentBoundaryCondition->setObjectIds(originalObjectIds);
    
    HydrodynamicDataDialog *hydrodynamicDataDialog = nullptr;
    
    for (QWidget *widget : QApplication::topLevelWidgets()) {
        if (widget->objectName() == "MainWindow") {
            hydrodynamicDataDialog = widget->findChild<HydrodynamicDataDialog*>("HydrodynamicDataDialog");
            break;
        }
    }
    
    HydrodynamicVTKWidget *vtkWidget = hydrodynamicDataDialog->findChild<HydrodynamicVTKWidget*>("vtkWidget");
    vtkWidget->clearBoundaryCondition(currentBoundaryCondition);
    hydrodynamicDataDialog->toggleWidgets(true);
    
    QDialog::closeEvent(event);
}
