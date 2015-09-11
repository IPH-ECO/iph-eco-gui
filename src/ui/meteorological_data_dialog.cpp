#include "include/ui/meteorological_data_dialog.h"
#include "ui_meteorological_data_dialog.h"

MeteorologicalDataDialog::MeteorologicalDataDialog(QWidget *parent) : QDialog(parent), ui(new Ui::MeteorologicalDataDialog) {
	ui->setupUi(this);
}