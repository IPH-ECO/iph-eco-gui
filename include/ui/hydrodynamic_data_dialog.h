#ifndef HYDRODYNAMIC_DATA_DIALOG_H
#define HYDRODYNAMIC_DATA_DIALOG_H

#include "include/repository/hydrodynamic_data_seed.h"

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
    HydrodynamicDataSeed *seed;
    
    void setupItems();
    void expandTrees();
    void buildParametersMap();
    bool isCheckable(QTreeWidgetItem *item) const;
    bool isCheckableProcess(QTreeWidgetItem *item) const;
    bool hasChildChecked(QTreeWidgetItem *item, QTreeWidgetItem *excludedItem) const;
    QString getCheckableProcessIdentifier(QTreeWidgetItem *item);
private slots:
    void on_trwProcesses_itemChanged(QTreeWidgetItem *item, int column);
    void on_btnDone_clicked();
public:
    explicit HydrodynamicDataDialog(QWidget *parent);
    ~HydrodynamicDataDialog();
};

class HydrodynamicDataNode {
private:
    QTreeWidgetItem *item;
    bool siblingsVisible;
    
public:
    HydrodynamicDataNode() {}
    HydrodynamicDataNode(QTreeWidgetItem *item, bool siblingsVisible = false) : item(item), siblingsVisible(siblingsVisible) {}
    
    QTreeWidgetItem* getItem() const {
        return item;
    }
    
    void setItem(QTreeWidgetItem *item) {
        this->item = item;
    }
    
    bool isSiblingsVisible() const {
        return siblingsVisible;
    }
    
    void setSiblingsVisible(bool siblingsVisible) {
        this->siblingsVisible = siblingsVisible;
    }
};

Q_DECLARE_METATYPE(HydrodynamicDataNode)

#endif // HYDRODYNAMIC_DATA_DIALOG_H
