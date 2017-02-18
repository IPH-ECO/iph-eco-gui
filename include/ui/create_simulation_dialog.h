#ifndef CREATE_SIMULATION_DIALOG_H
#define CREATE_SIMULATION_DIALOG_H

#include <domain/simulation.h>

#include <QDialog>
#include <QWidget>

namespace Ui {
	class CreateSimulationDialog;
}

class CreateSimulationDialog : public QDialog {
	Q_OBJECT
private:
	Ui::CreateSimulationDialog *ui;

	bool isValid();

    virtual void accept();
    uint calculateMaximumTimestamp(const QList<TimeSeries*> &timeSeriesList);
    uint calculateMinimumTimestamp(const QList<TimeSeries*> &timeSeriesList);

public:
	explicit CreateSimulationDialog(QWidget *parent = 0);
    ~CreateSimulationDialog();

private slots:
    void on_btnAddLayer_clicked();
    void on_btnRemoveLayer_clicked();
    void on_cbxHydrodynamic_currentTextChanged(const QString &configurationName);
    void on_cbxWaterQuality_currentTextChanged(const QString &configurationName);
    void on_btnBrowseOutputDirectory_clicked();
    void on_cbxTemplate_currentTextChanged(const QString &simulationLabel);
};

#endif // CREATE_SIMULATION_DIALOG_H
