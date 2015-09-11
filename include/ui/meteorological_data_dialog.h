#ifndef METEOROLOGICAL_DATA_DIALOG_H
#define METEOROLOGICAL_DATA_DIALOG_H

#include <QDialog>
#include <QWidget>

namespace Ui {
	class MeteorologicalDataDialog;
}

class MeteorologicalDataDialog : public QDialog {
	Q_OBJECT
public:
	explicit MeteorologicalDataDialog(QWidget *parent = 0);
private:
	Ui::MeteorologicalDataDialog *ui;
};

#endif // METEOROLOGICAL_DATA_DIALOG_H