#ifndef GRID_LAYER_ATTRIBUTES_DIALOG_H
#define GRID_LAYER_ATTRIBUTES_DIALOG_H

#include "include/domain/grid_data.h"

#include <QColor>
#include <QDialog>
#include <QToolButton>
#include <QAbstractButton>

namespace Ui {
    class GridLayerAttributesDialog;
}

class GridLayerAttributesDialog : public QDialog {
    Q_OBJECT
private:
    Ui::GridLayerAttributesDialog *ui;
    double defaultMapMinimum;
    double defaultMapMaximum;
    double defaultPointsMinimum;
    double defaultPointsMaximum;
    GridData *gridData;
    QColor currentLineColor;
    QToolButton *currentMapColorGradientButton;
    QToolButton *defaultMapColorGradientButton;
    QToolButton *currentPointsColorGradientButton;
    QToolButton *defaultPointsColorGradientButton;
    
    void setupMapTab();
    void setupPointsTab();
    void setupMeshTab();
    void setupColorGradientTemplates(QToolButton *&defaultButton, QToolButton *&currentButton, bool isMapTab);
    bool isValid();
private slots:
    void on_btnUseMapOriginalValues_clicked();
    void on_btnUsePointsOriginalValues_clicked();
    void on_btnLineColor_clicked();
    void colorGradientButtonClicked(bool checked);
    void on_buttonBox_clicked(QAbstractButton *button);
public:
    explicit GridLayerAttributesDialog(QWidget *parent, GridData *gridData);
    ~GridLayerAttributesDialog();
};

#endif // GRID_LAYER_ATTRIBUTES_DIALOG_H