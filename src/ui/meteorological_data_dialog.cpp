#include "include/ui/meteorological_data_dialog.h"
#include "ui_meteorological_data_dialog.h"
#include "include/application/iph_application.h"
#include "include/domain/grid_data_configuration.h"
#include "include/ui/time_series_dialog.h"

#include <QSet>
#include <QFile>
#include <QMessageBox>
#include <QMdiSubWindow>
#include <QStandardItemModel>
#include <QTreeWidgetItemIterator>
#include <GeographicLib/GeoCoords.hpp>

MeteorologicalDataDialog::MeteorologicalDataDialog(QWidget *parent) :
    QDialog(parent), ui(new Ui::MeteorologicalDataDialog), unsavedConfiguration(new MeteorologicalConfiguration), currentConfiguration(unsavedConfiguration), currentStation(nullptr)
{
	ui->setupUi(this);
    
    Project *project = IPHApplication::getCurrentProject();
    QSet<HydrodynamicConfiguration*> hydrodynamicConfigurations = project->getHydrodynamicConfigurations();
    QSet<GridDataConfiguration*> gridDataConfigurations = project->getGridDataConfigurations();
    
    ui->cbxConfiguration->blockSignals(true);
    for (HydrodynamicConfiguration *configuration : hydrodynamicConfigurations) {
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
    
    ui->stationGroupBox->hide();
    ui->parameterGroupBox->hide();
    ui->btnApplyStation->hide();
    ui->btnApplyParameter->hide();
    
    toggleModelingOption(true);
}

void MeteorologicalDataDialog::toggleModelingOption(bool disable) {
    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->cbxFunction->model());
    QStandardItem *item = model->item(2);
    
    item->setFlags(disable ? ~(item->flags() & Qt::ItemIsSelectable|Qt::ItemIsEnabled) : (Qt::ItemIsSelectable|Qt::ItemIsEnabled));
}

void MeteorologicalDataDialog::on_cbxGridData_currentIndexChanged(const QString &name) {
    bool isNameEmpty = name.isEmpty();
    
    if (!isNameEmpty) {
        Project *project = IPHApplication::getCurrentProject();
        Mesh *mesh = project->getGridDataConfiguration(name)->getMesh();
        
        ui->vtkWidget->render(mesh);
    }
    
    ui->btnAddStation->setDisabled(isNameEmpty);
    ui->btnRemoveStation->setDisabled(isNameEmpty);
}

void MeteorologicalDataDialog::on_trStations_itemSelectionChanged() {
    QTreeWidgetItem *selectedItem = ui->trStations->currentItem();
    bool isParameterItem = selectedItem->parent();
    
    if (isParameterItem) {
        MeteorologicalParameter *parameter = qvariant_cast<MeteorologicalParameter*>(selectedItem->data(0, Qt::UserRole));
        MeteorologicalParameterFunction function = parameter->getFunction();
        bool isConstant = function == MeteorologicalParameterFunction::CONSTANT;
        bool isWindParameter = parameter->getName() == "Wind";
        
        toggleModelingOption(parameter->getName() != "Evaporation");
        
        ui->stationGroupBox->hide();
        ui->cbxFunction->setCurrentText(parameter->getFunctionStr());
        ui->edtConstant->setText(QString::number(parameter->getConstantValue()));
        ui->edtConstant->setVisible(isConstant && !isWindParameter);
        ui->lblValue->setVisible(isConstant && !isWindParameter);
        ui->lblEntries->setVisible(!isConstant);
        ui->btnShowTimeSeries->setVisible(!isConstant);
        ui->lblType->setVisible(isWindParameter);
        ui->cbxType->setVisible(isWindParameter);
        ui->lblXComponent->setVisible(isWindParameter && ui->cbxType->currentText() == "XY Components");
        ui->edtXComponent->setVisible(isWindParameter && ui->cbxType->currentText() == "XY Components");
        ui->lblYComponent->setVisible(isWindParameter && ui->cbxType->currentText() == "XY Components");
        ui->edtYComponent->setVisible(isWindParameter && ui->cbxType->currentText() == "XY Components");
        ui->lblIntensity->setVisible(isWindParameter && ui->cbxType->currentText() == "Intensity and Direction");
        ui->edtIntensity->setVisible(isWindParameter && ui->cbxType->currentText() == "Intensity and Direction");
        ui->lblDirection->setVisible(isWindParameter && ui->cbxType->currentText() == "Intensity and Direction");
        ui->edtDirection->setVisible(isWindParameter && ui->cbxType->currentText() == "Intensity and Direction");
        ui->parameterGroupBox->setTitle(QString("%1 (%2)").arg(parameter->getName()).arg(parameter->getUnit()));
        ui->parameterGroupBox->show();
        ui->btnApplyParameter->show();
        ui->btnApplyStation->hide();
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
        ui->btnApplyParameter->hide();
        ui->btnApplyStation->show();
        ui->vtkWidget->highlightStation(currentStation);
    }
    
    ui->btnRemoveStation->setDisabled(isParameterItem);
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
}

void MeteorologicalDataDialog::on_btnRemoveStation_clicked() {
    QTreeWidgetItem *item = ui->trStations->currentItem();
    
    if (item) {
        MeteorologicalStation *station = qvariant_cast<MeteorologicalStation*>(item->data(0, Qt::UserRole));
        QString question = QString("Are you sure you want to remove station '%1'?").arg(station->getName());
        
        if (QMessageBox::question(this, tr("Meteorological Data"), question) == QMessageBox::Yes) {
            ui->vtkWidget->removeStation(station);
            ui->trStations->blockSignals(true);
            ui->trStations->takeTopLevelItem(ui->trStations->currentIndex().row());
            ui->trStations->blockSignals(false);
            currentConfiguration->removeStation(station);
            
            if (!ui->trStations->currentItem()) {
                ui->stationGroupBox->hide();
                ui->btnApplyStation->hide();
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
    
    if (!currentItem) {
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
        QTreeWidgetItem *stationItem = new QTreeWidgetItem({ currentStation->getName() });
        QList<MeteorologicalParameter*> parameters = MeteorologicalParameter::createDefaultParameters();
        
        stationItem->setData(0, Qt::UserRole, QVariant::fromValue(currentStation));
        
        for (MeteorologicalParameter *parameter : parameters) {
            QTreeWidgetItem *item = new QTreeWidgetItem({ parameter->getName() });
            item->setData(0, Qt::UserRole, QVariant::fromValue(parameter));
            stationItem->addChild(item);
        }
        
        ui->trStations->addTopLevelItem(stationItem);
        ui->trStations->setCurrentItem(stationItem);
    }
    
    ui->vtkWidget->addStation(currentStation);
}

void MeteorologicalDataDialog::on_btnApplyParameter_clicked() {
    QTreeWidgetItem *item = ui->trStations->currentItem();
    
    if (item) {
        MeteorologicalParameter *parameter = qvariant_cast<MeteorologicalParameter*>(item->data(0, Qt::UserRole));
        bool isWindParameter = parameter->getName() == "Wind";
        
        parameter->setFunction(ui->cbxFunction->currentText());
        
        if (isWindParameter) {
            if (ui->cbxType->currentText() == "XY Components") {
                parameter->setXComponent(ui->edtXComponent->text().toDouble());
                parameter->setYComponent(ui->edtYComponent->text().toDouble());
            } else {
                parameter->setIntensity(ui->edtIntensity->text().toDouble());
                parameter->setDirection(ui->edtDirection->text().toDouble());
            }
        }
    }
}

void MeteorologicalDataDialog::on_cbxFunction_currentIndexChanged(const QString &function) {
    bool isConstant = function == "Constant";
    bool isTimeSeries = function == "Time Series";
    
    ui->lblValue->setVisible(isConstant);
    ui->edtConstant->setVisible(isConstant);
    ui->lblIntensity->setVisible(isConstant);
    ui->edtIntensity->setVisible(isConstant);
    ui->lblDirection->setVisible(isConstant);
    ui->edtDirection->setVisible(isConstant);
    ui->lblEntries->setVisible(isTimeSeries);
    ui->btnShowTimeSeries->setVisible(isTimeSeries);
}

void MeteorologicalDataDialog::on_btnClose_clicked() {
    QMdiSubWindow *parentWindow = static_cast<QMdiSubWindow*>(parent());
    parentWindow->close();
}

void MeteorologicalDataDialog::on_cbxType_currentIndexChanged(const QString &type) {
    bool isXyComponents = type == "XY Components";
    
    ui->lblXComponent->setVisible(isXyComponents);
    ui->edtXComponent->setVisible(isXyComponents);
    ui->lblYComponent->setVisible(isXyComponents);
    ui->edtYComponent->setVisible(isXyComponents);
    ui->lblIntensity->setVisible(!isXyComponents);
    ui->edtIntensity->setVisible(!isXyComponents);
    ui->lblDirection->setVisible(!isXyComponents);
    ui->edtDirection->setVisible(!isXyComponents);
}

void MeteorologicalDataDialog::on_btnShowTimeSeries_clicked() {
    TimeSeriesType timeSeriesType = TimeSeries::mapStringToEnumType(ui->cbxType->currentText());
    TimeSeriesDialog *timeSeriesDialog = new TimeSeriesDialog(this, timeSeriesList, timeSeriesType);
    int exitCode = timeSeriesDialog->exec();

    if (exitCode == QDialog::Accepted) {
        timeSeriesList = timeSeriesDialog->getTimeSeriesList();
    }
}