#include <ui/water_quality_parameter_dialog.h>
#include <ui_water_quality_parameter_dialog.h>

WaterQualityParameterDialog::WaterQualityParameterDialog(QWidget *parent, const QString &parameterName, const QStringList &distribuition) :
	QDialog(parent),
	ui(new Ui::WaterQualityParameterDialog())
{
	ui->setupUi(this);

    QFont font = ui->lblParameter->font();
    font.setBold(true);
    ui->lblParameter->setFont(font);
    ui->lblParameter->setText(parameterName);
}