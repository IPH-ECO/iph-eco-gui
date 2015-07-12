#ifndef HYDRODYNAMIC_DATA_DIALOG_H
#define HYDRODYNAMIC_DATA_DIALOG_H

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
    
    void hideParametersItems();
    void setupParametersTree();
    void setupDefaultItems();
    void lookupParameters();
    bool isCheckableProcess(QTreeWidgetItem *item) const;
    bool hasChildChecked(QTreeWidgetItem *item, QTreeWidgetItem *excludedItem) const;
    void toggleParameterItem(QTreeWidgetItem *item);
private slots:
    void on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column);
public:
    explicit HydrodynamicDataDialog(QWidget *parent);
    ~HydrodynamicDataDialog();
};

#endif // HYDRODYNAMIC_DATA_DIALOG_H