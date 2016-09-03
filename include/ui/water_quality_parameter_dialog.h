#ifndef WATER_QUALITY_PARAMETER_DIALOG_H
#define WATER_QUALITY_PARAMETER_DIALOG_H

#include <domain/water_quality_configuration.h>

#include <QDialog>
#include <QWidget>
#include <QTableWidgetItem>

namespace Ui {
	class WaterQualityParameterDialog;
}

class WaterQualityParameterDialog : public QDialog {
    Q_OBJECT
private:
	Ui::WaterQualityParameterDialog *ui;
    WaterQualityConfiguration *configuration;
    WaterQualityParameter *parameter;
public:
	WaterQualityParameterDialog(QWidget *parent, WaterQualityConfiguration *configuration, WaterQualityParameter *parameter, const QList<int> &distribuition);
    ~WaterQualityParameterDialog();
    
    virtual void accept();
};

#endif // WATER_QUALITY_PARAMETER_DIALOG_H