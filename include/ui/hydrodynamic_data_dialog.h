#ifndef HYDRODYNAMIC_DATA_DIALOG_H
#define HYDRODYNAMIC_DATA_DIALOG_H

#include "include/domain/hydrodynamic_configuration.h"
#include "include/repository/hydrodynamic_data_repository.h"

#include <QDialog>
#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
    class HydrodynamicDataDialog;
}

class HydrodynamicDataDialog : public QDialog {
    Q_OBJECT
private:
    Ui::HydrodynamicDataDialog *ui;
    HydrodynamicDataRepository *hydrodynamicDataRepository;
    HydrodynamicConfiguration *unsavedConfiguration;
    HydrodynamicConfiguration *currentConfiguration;
    
    void setupItems();
    void expandTrees();
private slots:
    void on_cbxConfiguration_currentIndexChanged(const QString &configurationName);
    void on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column);
    void on_btnDone_clicked();
    void on_btnSave_clicked();
public:
    explicit HydrodynamicDataDialog(QWidget *parent);
    ~HydrodynamicDataDialog();
};

#endif // HYDRODYNAMIC_DATA_DIALOG_H
