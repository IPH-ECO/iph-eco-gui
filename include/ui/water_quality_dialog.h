#ifndef WATER_QUALITY_DIALOG_H
#define WATER_QUALITY_DIALOG_H

#include <QDialog>
#include <QWidget>

namespace Ui {
	class WaterQualityDialog;
}

class WaterQualityDialog : public QDialog {
	Q_OBJECT
private:
	Ui::WaterQualityDialog *ui;
private slots:
    void on_btnHide_clicked();
public:
	explicit WaterQualityDialog(QWidget *parent = 0);
};

#endif // WATER_QUALITY_DIALOG_H