#ifndef HYDRODYNAMIC_DATA_DIALOG_H
#define HYDRODYNAMIC_DATA_DIALOG_H

#include <domain/hydrodynamic_configuration.h>
#include <repository/hydrodynamic_data_repository.h>
#include "abstract_mesh_dialog.h"
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

class HydrodynamicDataDialog : public AbstractMeshDialog {
    Q_OBJECT
    
    friend class BoundaryConditionDialog;
private:
    Ui::HydrodynamicDataDialog *ui;
    HydrodynamicDataRepository *hydrodynamicDataRepository;
    HydrodynamicConfiguration *unsavedConfiguration;
    HydrodynamicConfiguration *currentConfiguration;
    GridDataConfiguration *currentGridDataConfiguration;
    BoundaryConditionDialog *boundaryConditionDialog;
    
    void addParameterItemWidget(HydrodynamicParameter *parameter);
    void setupItems();
    void expandParameterTree();
    void closeEvent(QCloseEvent *event);
    void toggleWidgets(bool enable);
    void clearLayout(QLayout *layout);
private slots:
    void on_cbxConfiguration_currentIndexChanged(const QString &configurationName);
    void on_cbxGridDataConfiguration_currentIndexChanged(const QString &meshName);
    void on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column);
    void on_btnAddBoundaryCondition_clicked();
    void on_btnEditBoundaryCondition_clicked();
    void on_btnRemoveBoundaryCondition_clicked();
    void on_btnRemoveConfiguration_clicked();
    void on_btnNewConfiguration_clicked();
    void on_btnApplyConfiguration_clicked();
    void on_tblBoundaryConditions_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    virtual void onToggleLabelsClicked(bool checked);
public:
    explicit HydrodynamicDataDialog(QWidget *parent);
    ~HydrodynamicDataDialog();
    bool isCellLabelsActionChecked() const;
    void toggleZoomAreaAction(bool enable);
};

#endif // HYDRODYNAMIC_DATA_DIALOG_H
