#include <ui/vertical_integrated_boundary_condition_dialog.h>
#include "ui_vertical_integrated_boundary_condition_dialog.h"

VerticalIntegratedBoundaryConditionDialog::VerticalIntegratedBoundaryConditionDialog(HydrodynamicConfiguration *configuration, HydrodynamicBoundaryCondition *boundaryCondition) :
    HydrodynamicBoundaryConditionDialog(configuration, boundaryCondition),
    ui(new Ui::VerticalIntegratedBoundaryConditionDialog)
{
    ui->setupUi(this);
    this->setupUi();
}

VerticalIntegratedBoundaryConditionDialog::~VerticalIntegratedBoundaryConditionDialog() {
    delete ui;
}

void VerticalIntegratedBoundaryConditionDialog::accept() {
    if (!isValid()) {
        return;
    }
    
    BoundaryConditionType boundaryConditionType = BoundaryConditionType::WATER_LEVEL;
    
    if (ui->cbxType->currentText() == "Water Flow") {
        boundaryConditionType = BoundaryConditionType::WATER_FLOW;
    } else if (ui->cbxType->currentText() == "Normal Depth") {
        boundaryConditionType = BoundaryConditionType::NORMAL_DEPTH;
    }
    
    currentBoundaryCondition->setName(ui->edtName->text());
    currentBoundaryCondition->setType(boundaryConditionType);
    currentBoundaryCondition->setObjectIds(ui->lblElementIds->text());
    currentBoundaryCondition->setInputModule(InputModule::HYDRODYNAMIC);
    
    configuration->addBoundaryCondition(currentBoundaryCondition);
    
    HydrodynamicBoundaryConditionDialog::accept();
}

bool VerticalIntegratedBoundaryConditionDialog::isValid() {
    return true;
}
