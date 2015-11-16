#ifndef LAYER_PROPERTIES_DIALOG_H
#define LAYER_PROPERTIES_DIALOG_H

#include "include/domain/layer_properties.h"

#include <QColor>
#include <QDialog>
#include <QToolButton>
#include <QAbstractButton>

namespace Ui {
	class LayerPropertiesDialog;
}

class LayerPropertiesDialog : public QDialog {
    Q_OBJECT
private:
    Ui::LayerPropertiesDialog *ui;
    LayerProperties *layerProperties;
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
    LayerPropertiesDialog(QWidget *parent, LayerProperties *layerProperties);
    ~LayerPropertiesDialog();
    
    void removeMeshTab();
signals:
    void applyChanges();
};

#endif // LAYER_PROPERTIES_DIALOG_H