#include <ui/meteorological_data_dialog.h>
#include "ui_meteorological_data_dialog.h"
#include <application/iph_application.h>
#include <domain/grid_data_configuration.h>
#include <ui/time_series_dialog.h>

#include <QSet>
#include <QFile>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QStandardItemModel>
#include <QTreeWidgetItemIterator>
#include <GeographicLib/GeoCoords.hpp>

MeteorologicalDataDialog::MeteorologicalDataDialog(QWidget *parent) :
    AbstractMeshDialog(parent), ui(new Ui::MeteorologicalDataDialog),
    unsavedConfiguration(new MeteorologicalConfiguration), currentConfiguration(unsavedConfiguration), currentStation(nullptr)
{
	ui->setupUi(this);
    this->vtkWidget = ui->vtkWidget;
    
    Project *project = IPHApplication::getCurrentProject();
    QSet<MeteorologicalConfiguration*> meteorologicalConfigurations = project->getMeteorologicalConfigurations();
    QSet<GridDataConfiguration*> gridDataConfigurations = project->getGridDataConfigurations();
    
    ui->cbxConfiguration->blockSignals(true);
    for (MeteorologicalConfiguration *configuration : meteorologicalConfigurations) {
        ui->cbxConfiguration->addItem(configuration->getName());
    }
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->cbxConfiguration->blockSignals(false);
    
    ui->cbxGridData->blockSignals(true);
    for (GridDataConfiguration *configuration : gridDataConfigurations) {
        ui->cbxGridData->addItem(configuration->getName());
    }
    ui->cbxGridData->setCurrentIndex(-1);
    ui->cbxGridData->blockSignals(false);
    
    ui->btnAddStation->setEnabled(false);
    ui->btnRemoveStation->setEnabled(false);
    ui->stationGroupBox->hide();
    ui->parameterGroupBox->hide();
    
    toggleModelingOption(true);
}

void MeteorologicalDataDialog::toggleModelingOption(bool disable) {
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->cbxFunction->model());
    QStandardItem *item = model->item(2);
    
    item->setFlags(disable ? ~(item->flags() & Qt::ItemIsSelectable|Qt::ItemIsEnabled) : (Qt::ItemIsSelectable|Qt::ItemIsEnabled));
}

void MeteorologicalDataDialog::on_cbxGridData_currentIndexChanged(const QString &name) {
    bool isNameEmpty = name.isEmpty();
    
    if (isNameEmpty) {
        ui->stationGroupBox->hide();
        ui->parameterGroupBox->hide();
        ui->btnRemoveStation->setDisabled(true);
    } else {
        Project *project = IPHApplication::getCurrentProject();
        Mesh *mesh = project->getGridDataConfiguration(name)->getMesh();
        
        ui->vtkWidget->render(mesh);
    }
    
    ui->btnAddStation->setDisabled(isNameEmpty);
}

void MeteorologicalDataDialog::on_trStations_itemSelectionChanged() {
    QTreeWidgetItem *selectedItem = ui->trStations->currentItem();
    
    if (selectedItem) {
        bool isParameterItem = selectedItem->parent();
    
        if (isParameterItem) {
            MeteorologicalParameter *parameter = qvariant_cast<MeteorologicalParameter*>(selectedItem->data(0, Qt::UserRole));
            MeteorologicalParameterFunction function = parameter->getFunction();
            bool isConstant = function == MeteorologicalParameterFunction::CONSTANT;
            bool isWindParameter = parameter->getName() == "Wind";
            bool isEvaporation = parameter->getName() == "Evaporation";
            
            toggleModelingOption(!isEvaporation);
            
            ui->stationGroupBox->hide();
            ui->cbxFunction->setCurrentText(parameter->getFunctionStr());
            ui->edtConstant->setText(QString::number(parameter->getConstantValue()));
            ui->edtConstant->setVisible(isConstant && !isWindParameter);
            ui->lblValue->setVisible(isConstant && !isWindParameter);
            ui->lblEntries->setVisible(!isConstant && !isEvaporation);
            ui->btnShowTimeSeries->setVisible(!isConstant && !isEvaporation);
            ui->lblType->setVisible(isWindParameter);
            ui->cbxType->setVisible(isWindParameter);
            ui->cbxType->setCurrentIndex(!parameter->getUseXYComponent());
            ui->lblXComponent->setVisible(isWindParameter && isConstant && parameter->getUseXYComponent());
            ui->edtXComponent->setVisible(isWindParameter && isConstant && parameter->getUseXYComponent());
            ui->lblYComponent->setVisible(isWindParameter && isConstant && parameter->getUseXYComponent());
            ui->edtYComponent->setVisible(isWindParameter && isConstant && parameter->getUseXYComponent());
            ui->lblIntensity->setVisible(isWindParameter && isConstant && !parameter->getUseXYComponent());
            ui->edtIntensity->setVisible(isWindParameter && isConstant && !parameter->getUseXYComponent());
            ui->lblDirection->setVisible(isWindParameter && isConstant && !parameter->getUseXYComponent());
            ui->edtDirection->setVisible(isWindParameter && isConstant && !parameter->getUseXYComponent());
            if (parameter->getUseXYComponent()) {
                ui->edtXComponent->setText(QString::number(parameter->getXComponent()));
                ui->edtYComponent->setText(QString::number(parameter->getYComponent()));
            } else {
                ui->edtIntensity->setText(QString::number(parameter->getIntensity()));
                ui->edtDirection->setText(QString::number(parameter->getDirection()));
            }
            ui->parameterGroupBox->setTitle(QString("%1 (%2)").arg(parameter->getName()).arg(parameter->getUnit()));
            ui->parameterGroupBox->show();
            currentStation = nullptr;
        } else {
            currentStation = qvariant_cast<MeteorologicalStation*>(selectedItem->data(0, Qt::UserRole));
            
            ui->parameterGroupBox->hide();
            ui->edtStationName->setText(currentStation->getName());
            if (currentStation->getUseLatitudeLongitude()) {
                ui->edtX->setText(QString::number(currentStation->getLatitude()));
                ui->edtY->setText(QString::number(currentStation->getLongitude()));
                ui->rdoLatLong->setChecked(true);
            } else {
                ui->edtX->setText(QString::number(currentStation->getUtmX()));
                ui->edtY->setText(QString::number(currentStation->getUtmY()));
                ui->rdoUtm->setChecked(true);
            }
            
            ui->stationGroupBox->show();
            ui->vtkWidget->highlightStation(currentStation);
        }
        
        ui->btnApplyStation->setVisible(!isParameterItem);
        ui->btnApplyParameter->setVisible(isParameterItem);
        ui->btnRemoveStation->setDisabled(isParameterItem);
    } else {
        on_btnAddStation_clicked();
        ui->btnRemoveStation->setDisabled(true);
        ui->stationGroupBox->hide();
        ui->parameterGroupBox->hide();
    }
}

void MeteorologicalDataDialog::on_btnAddStation_clicked() {
    currentStation = nullptr;
    ui->trStations->clearSelection();
    ui->trStations->setCurrentItem(nullptr);
    ui->parameterGroupBox->hide();
    ui->edtStationName->setText("");
    ui->edtX->setText("");
    ui->edtY->setText("");
    ui->rdoUtm->setChecked(true);
    ui->btnApplyParameter->hide();
    ui->btnApplyStation->show();
    ui->stationGroupBox->show();
    ui->edtStationName->setFocus();
}

void MeteorologicalDataDialog::on_btnRemoveStation_clicked() {
    QTreeWidgetItem *item = ui->trStations->currentItem();
    
    if (item) {
        MeteorologicalStation *station = qvariant_cast<MeteorologicalStation*>(item->data(0, Qt::UserRole));
        QString question = QString("Are you sure you want to remove station '%1'?").arg(station->getName());
        
        if (QMessageBox::question(this, tr("Meteorological Data"), question) == QMessageBox::Yes) {
            ui->vtkWidget->removeStation(station);
            ui->trStations->takeTopLevelItem(ui->trStations->currentIndex().row());
            currentConfiguration->removeStation(station);
            
            if (!ui->trStations->currentItem()) {
                ui->stationGroupBox->hide();
            }
        }
    }
}

void MeteorologicalDataDialog::on_rdoUtm_clicked(bool checked) {
    if (checked) {
        ui->lblX->setText("UTM-X");
        ui->lblY->setText("UTM-Y");
    }
}

void MeteorologicalDataDialog::on_rdoLatLong_clicked(bool checked) {
    if (checked) {
        ui->lblX->setText("Lat");
        ui->lblY->setText("Long");
    }
}

void MeteorologicalDataDialog::on_btnApplyStation_clicked() {
    QTreeWidgetItem *currentItem = ui->trStations->currentItem();
    
    if (ui->edtStationName->text().isEmpty()) {
        QMessageBox::warning(this, tr("Meteorological Data"), tr("Station name can't be blank."));
        return;
    }
    
    QTreeWidgetItemIterator it(ui->trStations, QTreeWidgetItemIterator::HasChildren);
    
    while (*it) {
        QTreeWidgetItem *item = *it;
        
        if (item->text(0) == ui->edtStationName->text() && item != currentItem) {
            QMessageBox::warning(this, tr("Meteorological Data"), tr("Station name already in use."));
            return;
        }
        
        it++;
    }
    
    if (ui->edtX->text().isEmpty() || ui->edtY->text().isEmpty()) {
        QMessageBox::warning(this, tr("Meteorological Data"), tr("Station coordinate can't be blank."));
        return;
    }
    
    double latitude = ui->edtX->text().toDouble();
    double longitude = ui->edtY->text().toDouble();
    double utmX = latitude;
    double utmY = longitude;
    bool useLatitudeLongitude = ui->rdoLatLong->isChecked();
    
    if (!currentStation) {
        currentStation = new MeteorologicalStation(ui->edtStationName->text());
    }
    
    if (useLatitudeLongitude) {
        currentStation->setLatitude(latitude);
        currentStation->setLongitude(longitude);
        
        try {
            GeographicLib::GeoCoords utmCoordinate(latitude, longitude);
            utmX = utmCoordinate.Easting();
            utmY = utmCoordinate.Northing();
        } catch (const GeographicLib::GeographicErr &e) {
            QMessageBox::warning(this, tr("Meteorological Data"), e.what());
            return;
        }
    }
    
    currentStation->setUtmX(utmX);
    currentStation->setUtmY(utmY);
    
    if (currentItem) {
        currentItem->setText(0, ui->edtStationName->text());
    } else {
        QTreeWidgetItem *stationItem = new QTreeWidgetItem(QStringList(currentStation->getName()));
        QList<MeteorologicalParameter*> parameters = MeteorologicalParameter::createDefaultParameters();
        
        currentStation->setParameters(parameters);
        stationItem->setData(0, Qt::UserRole, QVariant::fromValue(currentStation));
        
        for (MeteorologicalParameter *parameter : parameters) {
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList(parameter->getName()));
            item->setData(0, Qt::UserRole, QVariant::fromValue(parameter));
            stationItem->addChild(item);
        }
        
        ui->trStations->addTopLevelItem(stationItem);
        ui->trStations->setCurrentItem(stationItem);
    }
    
    currentConfiguration->addStation(currentStation);
    ui->vtkWidget->addStation(currentStation);
}

void MeteorologicalDataDialog::on_btnApplyParameter_clicked() {
    QTreeWidgetItem *item = ui->trStations->currentItem();
    
    if (item) {
        MeteorologicalParameter *parameter = qvariant_cast<MeteorologicalParameter*>(item->data(0, Qt::UserRole));
        bool useXYComponent = ui->cbxType->currentText() == "XY Components";
        
        if (ui->cbxFunction->currentText() == "Constant") {
            bool isWindParameter = parameter->getName() == "Wind";
            
            if (isWindParameter) {
                if (useXYComponent) {
                    if (ui->edtXComponent->text().isEmpty() || ui->edtYComponent->text().isEmpty()) {
                        QMessageBox::warning(this, tr("Meteorological Data"), tr("Invalid parameter values."));
                        return;
                    }
                    parameter->setXComponent(ui->edtXComponent->text().toDouble());
                    parameter->setYComponent(ui->edtYComponent->text().toDouble());
                } else {
                    if (ui->edtIntensity->text().isEmpty() || ui->edtDirection->text().isEmpty()) {
                        QMessageBox::warning(this, tr("Meteorological Data"), tr("Invalid parameter values."));
                        return;
                    }
                    parameter->setIntensity(ui->edtIntensity->text().toDouble());
                    parameter->setDirection(ui->edtDirection->text().toDouble());
                }
            } else {
                parameter->setConstantValue(ui->edtConstant->text().toDouble());
            }
        }
        
        parameter->setUseXYComponent(useXYComponent);
        parameter->setFunction(ui->cbxFunction->currentText());
    }
}

void MeteorologicalDataDialog::on_cbxFunction_currentIndexChanged(const QString &function) {
    bool isConstant = function == "Constant";
    bool isTimeSeries = function == "Time Series";
    bool isWind = ui->trStations->currentItem()->text(0) == "Wind";
    bool isXYComponent = ui->cbxType->currentText() == "XY Components";
    
    ui->lblValue->setVisible(isConstant && !isWind);
    ui->edtConstant->setVisible(isConstant && !isWind);
    ui->lblIntensity->setVisible(isConstant && isWind);
    ui->edtIntensity->setVisible(isConstant && isWind);
    ui->lblDirection->setVisible(isConstant && isWind);
    ui->edtDirection->setVisible(isConstant && isWind);
    ui->lblXComponent->setVisible(isConstant && isWind && isXYComponent);
    ui->edtXComponent->setVisible(isConstant && isWind && isXYComponent);
    ui->lblYComponent->setVisible(isConstant && isWind && isXYComponent);
    ui->edtYComponent->setVisible(isConstant && isWind && isXYComponent);
    ui->lblIntensity->setVisible(isConstant && isWind && !isXYComponent);
    ui->edtIntensity->setVisible(isConstant && isWind && !isXYComponent);
    ui->lblDirection->setVisible(isConstant && isWind && !isXYComponent);
    ui->edtDirection->setVisible(isConstant && isWind && !isXYComponent);
    ui->lblEntries->setVisible(isTimeSeries);
    ui->btnShowTimeSeries->setVisible(isTimeSeries);
    
    MeteorologicalParameter *parameter = qvariant_cast<MeteorologicalParameter*>(ui->trStations->currentItem()->data(0, Qt::UserRole));
    
    if (!isTimeSeries && !parameter->getTimeSeriesListPointer()->isEmpty() && parameter->getFunction() == MeteorologicalParameterFunction::TIME_SERIES) {
        QString question = tr("Would you like to remove %1 time series entries?").arg(parameter->getName());
        QMessageBox::StandardButton button = QMessageBox::question(this, tr("Meteorological Data"), question);
        
        if (button == QMessageBox::Yes) {
            parameter->getTimeSeriesListPointer()->clear();
        }
    }
    
    parameter->setFunction(function);
}

void MeteorologicalDataDialog::on_cbxType_currentIndexChanged(const QString &type) {
    bool isXyComponents = type == "XY Components";
    bool isTimeSeries = ui->cbxFunction->currentText() == "Time Series";
    
    ui->lblXComponent->setVisible(isXyComponents && !isTimeSeries);
    ui->edtXComponent->setVisible(isXyComponents && !isTimeSeries);
    ui->lblYComponent->setVisible(isXyComponents && !isTimeSeries);
    ui->edtYComponent->setVisible(isXyComponents && !isTimeSeries);
    ui->lblIntensity->setVisible(!isXyComponents && !isTimeSeries);
    ui->edtIntensity->setVisible(!isXyComponents && !isTimeSeries);
    ui->lblDirection->setVisible(!isXyComponents && !isTimeSeries);
    ui->edtDirection->setVisible(!isXyComponents && !isTimeSeries);
}

void MeteorologicalDataDialog::on_btnShowTimeSeries_clicked() {
    bool isWind = ui->trStations->currentItem()->text(0) == "Wind";
    MeteorologicalParameter *parameter = qvariant_cast<MeteorologicalParameter*>(ui->trStations->currentItem()->data(0, Qt::UserRole));
    TimeSeriesType timeSeriesType = isWind ? TimeSeries::mapStringToEnumType(ui->cbxType->currentText()) : TimeSeriesType::DEFAULT;
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this, timeSeriesType);

    timeSeriesDialog->loadTimeSeriesList(parameter->getTimeSeriesListPointer());
    timeSeriesDialog->setMeteorologicalParameter(parameter);
    int exitCode = timeSeriesDialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        parameter->setTimeSeriesChanged(timeSeriesDialog->hasChanges());
    }
}

void MeteorologicalDataDialog::on_btnNewConfiguration_clicked() {
    currentConfiguration = unsavedConfiguration;
    ui->cbxConfiguration->setCurrentIndex(-1);
    ui->edtConfigurationName->setText("");
    ui->trStations->clear();
    ui->cbxGridData->setCurrentIndex(-1);
    ui->vtkWidget->clear();
}

void MeteorologicalDataDialog::on_btnApplyConfiguration_clicked() {
    QString oldConfigurationName = ui->cbxConfiguration->currentText();
    QString newConfigurationName = ui->edtConfigurationName->text();
    
    if (newConfigurationName.isEmpty()) {
        QMessageBox::warning(this, tr("Meteorological Data"), tr("Configuration name can't be empty."));
        return;
    }
    
    if (ui->cbxGridData->currentIndex() == -1) {
        QMessageBox::warning(this, tr("Meteorological Data"), tr("This configuration must be associated to a Grid Data."));
        return;
    }
    
    if (ui->trStations->topLevelItemCount() == 0) {
        QMessageBox::warning(this, tr("Meteorological Data"), tr("This configuration must have at least one station."));
        return;
    }
    
    if (oldConfigurationName.isEmpty()) { // new mesh
        Project *project = IPHApplication::getCurrentProject();
        
        if (project->getMeteorologicalConfiguration(newConfigurationName)) {
            QMessageBox::critical(this, tr("Meteorological Data"), tr("Configuration name already used."));
            return;
        }
        
        GridDataConfiguration *gridDataConfiguration = project->getGridDataConfiguration(ui->cbxGridData->currentText());
        
        currentConfiguration->setName(newConfigurationName);
        currentConfiguration->setGridDataConfiguration(gridDataConfiguration);
        project->addMeteorologicalConfiguration(currentConfiguration);
        unsavedConfiguration = new MeteorologicalConfiguration();
        
        ui->cbxConfiguration->addItem(newConfigurationName);
        ui->cbxConfiguration->setCurrentText(newConfigurationName);
    } else {
        currentConfiguration->setName(newConfigurationName);
        ui->cbxConfiguration->setItemText(ui->cbxConfiguration->currentIndex(), newConfigurationName);
    }
}

void MeteorologicalDataDialog::on_btnRemoveConfiguration_clicked() {
    QString configurationName = ui->cbxConfiguration->currentText();
    
    if (configurationName.isEmpty()) {
        return;
    }
    
    QString questionStr = QString("Are you sure you want to remove '%1'?").arg(configurationName);
    QMessageBox::StandardButton questionBtn = QMessageBox::question(this, tr("Remove mesh"), questionStr);
    
    if (questionBtn == QMessageBox::Yes) {
        IPHApplication::getCurrentProject()->removeMeteorologicalConfiguration(configurationName);
        ui->cbxConfiguration->removeItem(ui->cbxConfiguration->currentIndex());
        this->on_btnNewConfiguration_clicked();
    }
}

void MeteorologicalDataDialog::on_cbxConfiguration_currentIndexChanged(const QString &configurationName) {
    if (configurationName.isEmpty()) {
        return;
    }
    
    currentConfiguration = IPHApplication::getCurrentProject()->getMeteorologicalConfiguration(configurationName);
    
    ui->edtConfigurationName->setText(currentConfiguration->getName());
    ui->cbxGridData->setCurrentText(currentConfiguration->getGridDataConfiguration()->getName());
    ui->trStations->clear();
    
    for (MeteorologicalStation *station : currentConfiguration->getStations()) {
        QTreeWidgetItem *stationItem = new QTreeWidgetItem(QStringList(station->getName()));
        stationItem->setData(0, Qt::UserRole, QVariant::fromValue(station));
        
        for (MeteorologicalParameter *parameter : station->getParameters()) {
            QTreeWidgetItem *parameterItem = new QTreeWidgetItem(QStringList(parameter->getName()));
            parameterItem->setData(0, Qt::UserRole, QVariant::fromValue(parameter));
            stationItem->addChild(parameterItem);
        }
        
        ui->trStations->addTopLevelItem(stationItem);
        ui->vtkWidget->addStation(station);
    }
}