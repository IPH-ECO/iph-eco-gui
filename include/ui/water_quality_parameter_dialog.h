#ifndef WATER_QUALITY_PARAMETER_DIALOG_H
#define WATER_QUALITY_PARAMETER_DIALOG_H

#include <domain/water_quality_parameter.h>
#include <repository/water_quality_repository.h>

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
    WaterQualityParameter *parameter;
    WaterQualityRepository *waterQualityRepository;
public:
	WaterQualityParameterDialog(QWidget *parent, WaterQualityParameter *parameter, const QList<int> &distribuition);
    ~WaterQualityParameterDialog();
    
    virtual void accept();
};

#endif // WATER_QUALITY_PARAMETER_DIALOG_H