#ifndef HYDRODYNAMIC_DATA_DIALOG_H
#define HYDRODYNAMIC_DATA_DIALOG_H

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
    
    void setupItems();
    void expandTrees();
private slots:
    void on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column);
    void on_btnDone_clicked();
public:
    explicit HydrodynamicDataDialog(QWidget *parent);
    ~HydrodynamicDataDialog();
};

#endif // HYDRODYNAMIC_DATA_DIALOG_H
