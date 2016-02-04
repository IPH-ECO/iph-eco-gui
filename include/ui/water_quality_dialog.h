#ifndef WATER_QUALITY_DIALOG_H
#define WATER_QUALITY_DIALOG_H

#include <domain/water_quality_configuration.h>
#include <repository/water_quality_repository.h>

#include <QDialog>
#include <QWidget>
#include <QTreeWidgetItem>

namespace Ui {
	class WaterQualityDialog;
}

class WaterQualityDialog : public QDialog {
	Q_OBJECT
private:
	Ui::WaterQualityDialog *ui;
	WaterQualityConfiguration *unsavedConfiguration;
	WaterQualityConfiguration *currentConfiguration;
	WaterQualityRepository *waterQualityRepository;

	void setupItems(WaterQualityParameter *parameter);
private slots:
	void on_cbxConfiguration_currentIndexChanged(const QString &configurationName);
	void on_cbxGridDataConfiguration_currentIndexChanged(const QString &gridDataConfigurationName);
	void on_btnRemoveConfiguration_clicked();
	void on_btnNewConfiguration_clicked();
	void on_btnApplyConfiguration_clicked();
	void on_trwParameters_itemChanged(QTreeWidgetItem *item, int column);
public:
	explicit WaterQualityDialog(QWidget *parent = 0);
	~WaterQualityDialog();
};

#endif // WATER_QUALITY_DIALOG_H