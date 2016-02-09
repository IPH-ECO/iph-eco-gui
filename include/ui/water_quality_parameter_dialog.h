#ifndef WATER_QUALITY_PARAMETER_DIALOG_H
#define WATER_QUALITY_PARAMETER_DIALOG_H

#include <QDialog>
#include <QWidget>

namespace Ui {
	class WaterQualityParameterDialog;
}

class WaterQualityParameterDialog : public QDialog {
private:
	Ui::WaterQualityParameterDialog *ui;
public:
	explicit WaterQualityParameterDialog(QWidget *parent, const QString &parameterName, const QStringList &distribuition);
};

#endif // WATER_QUALITY_PARAMETER_DIALOG_H