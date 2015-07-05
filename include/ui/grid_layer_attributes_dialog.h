#ifndef GRID_LAYER_ATTRIBUTES_DIALOG_H
#define GRID_LAYER_ATTRIBUTES_DIALOG_H

#include "include/domain/grid_data.h"

#include <QColor>
#include <QDialog>
#include <QAbstractButton>

namespace Ui {
    class GridLayerAttributesDialog;
}

class GridLayerAttributesDialog : public QDialog {
    Q_OBJECT
private:
    Ui::GridLayerAttributesDialog *ui;
    double defaultMinimum;
    double defaultMaximum;
    GridData *gridData;
    QColor currentLineColor;
    
    bool isValid();
private slots:
    void on_btnUseOriginalValues_clicked();
    void on_btnLineColor_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
public:
    explicit GridLayerAttributesDialog(QWidget *parent, GridData *gridData);
    ~GridLayerAttributesDialog();
};

#endif // GRID_LAYER_ATTRIBUTES_DIALOG_H
