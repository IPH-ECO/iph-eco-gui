#ifndef HYDRODYNAMIC_DATA_DIALOG_H
#define HYDRODYNAMIC_DATA_DIALOG_H

#include "include/domain/hydrodynamic_configuration.h"
#include "include/repository/hydrodynamic_data_repository.h"
#include "include/application/iph_types.h"
#include "boundary_condition_dialog.h"

#include <QDialog>
#include <QWidget>
#include <QCloseEvent>
#include <QTreeWidgetItem>
#include <QTableWidgetItem>

namespace Ui {
    class HydrodynamicDataDialog;
}

class HydrodynamicVTKWidget;
class BoundaryConditionDialog;

class HydrodynamicDataDialog : public QDialog {
    Q_OBJECT
    
    friend class BoundaryConditionDialog;
private:
    Ui::HydrodynamicDataDialog *ui;
    HydrodynamicDataRepository *hydrodynamicDataRepository;
    HydrodynamicConfiguration *unsavedConfiguration;
    HydrodynamicConfiguration *currentConfiguration;
    Mesh *currentMesh;
    BoundaryConditionDialog *boundaryConditionDialog;
    
    void addParameterItemWidget(HydrodynamicParameter *parameter);
    void setupItems();
    void expandTrees();
    void closeEvent(QCloseEvent *event);
    void toggleWidgets(bool enable);
private slots:
    void on_cbxConfiguration_currentIndexChanged(const QString &configurationName);
    void on_cbxMesh_currentIndexChanged(const QString &meshName);
    void on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column);
    void on_btnAddBoundaryCondition_clicked();
    void on_btnEditBoundaryCondition_clicked();
    void on_btnRemoveBoundaryCondition_clicked();
    void on_btnRemove_clicked();
    void on_btnDone_clicked();
    void on_btnSave_clicked();
    void on_btnShowCellLabels_clicked(bool checked);
    void on_btnBackgroundColor_clicked();
    void on_tblBoundaryConditions_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
public:
    explicit HydrodynamicDataDialog(QWidget *parent);
    ~HydrodynamicDataDialog();
public slots:
    void setCoordinate(double &x, double &y);
};

#endif // HYDRODYNAMIC_DATA_DIALOG_H
