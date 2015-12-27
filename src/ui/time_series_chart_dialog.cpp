#include <ui/time_series_chart_dialog.h>
#include "ui_time_series_chart_dialog.h"

#include <utility/time_series_chart_mouse_interactor.h>
#include <ui/coordinate_file_dialog.h>

#include <QFile>
#include <vtkPen.h>
#include <QDateTime>
#include <vtkAxis.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <QMessageBox>
#include <QToolButton>
#include <vtkCellData.h>
#include <vtkRenderer.h>
#include <vtkStringArray.h>
#include <vtkDoubleArray.h>
#include <vtkContextScene.h>
#include <vtkGenericDataObjectReader.h>

TimeSeriesChartDialog::TimeSeriesChartDialog(QWidget *parent, SimulationVTKWidget *simulationVTKWidget) :
	QDialog(parent),
    BOUNDARY_DEFAULT_DIR_KEY("boundary_default_dir"),
	ui(new Ui::TimeSeriesChartDialog),
    simulationVTKWidget(simulationVTKWidget),
    view(vtkSmartPointer<vtkContextView>::New()),
    chart(vtkSmartPointer<vtkChartXY>::New())
{
    Simulation *simulation = simulationVTKWidget->getCurrentSimulation();
    
    ui->setupUi(this);
    ui->edtInitialFrame->setText("1");
    ui->edtFrameStep->setText("1");
    ui->edtFinalFrame->setText(QString::number(simulation->getOutputFiles().size()));
    
    view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    view->SetInteractor(ui->vtkWidget->GetInteractor());
    
    ui->vtkWidget->SetRenderWindow(view->GetRenderWindow());
    
    for (int i = 0; i < simulationVTKWidget->getNumberOfMapLayers(); i++) {
        ui->cbxLayer->addItem(QString::number(i + 1));
    }
    
    QToolButton *btnExportToCSV = new QToolButton(this);
    btnExportToCSV->setIcon(QIcon(":/icons/text-csv.png"));
    btnExportToCSV->setToolTip("Export chart data to CSV");
    ui->buttonBox->addButton(btnExportToCSV, QDialogButtonBox::ActionRole);
    connect(btnExportToCSV, SIGNAL(clicked()), this, SLOT(btnExportToCSV_clicked()));

    chart->SetShowLegend(true);
    view->GetScene()->AddItem(chart);
    
    appSettings = new QSettings(QApplication::organizationName(), QApplication::applicationName(), this);
}

TimeSeriesChartDialog::~TimeSeriesChartDialog() {
    delete ui;
}

void TimeSeriesChartDialog::on_chkImportShapefile_clicked(bool checked) {
    if (checked) {
        TimeSeriesChartMouseInteractor *timeSeriesInteractor = TimeSeriesChartMouseInteractor::SafeDownCast(simulationVTKWidget->GetInteractor()->GetInteractorStyle());
        vtkSmartPointer<vtkIdTypeArray> pickedCellsIds = timeSeriesInteractor->getCellIdArray(simulationVTKWidget->getLayerKey());
        
        if (pickedCellsIds) {
            QMessageBox::StandardButton button = QMessageBox::question(this, "Time Series Chart", "This action will remove the current picked cells. Are you sure?");
            
            if (button == QMessageBox::Cancel || button == QMessageBox::No) {
                ui->chkImportShapefile->setChecked(false);
                return;
            }
            
            timeSeriesInteractor->removePickedCells(simulationVTKWidget->getLayerKey());
            chart->ClearPlots();
        }
    }
    
    if (ui->btnPicker->isChecked()) {
        ui->btnPicker->setChecked(false);
    }
}

void TimeSeriesChartDialog::on_btnBrowseShapefile_clicked() {
    QString extensions = "Keyhole Markup Language file (*.kml);;Text file (*.txt *xyz)";
    CoordinateFileDialog *dialog = new CoordinateFileDialog(this, tr("Select a coordinate file"), getDefaultDirectory(), extensions);
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        ui->edtShapefile->setText(dialog->selectedFiles().first());
    }
}

void TimeSeriesChartDialog::on_btnPicker_toggled(bool checked) {
    TimeSeriesChartMouseInteractor *timeSeriesMouseInteractor = TimeSeriesChartMouseInteractor::SafeDownCast(simulationVTKWidget->GetInteractor()->GetInteractorStyle());

    if (checked) {
        timeSeriesMouseInteractor->activatePicker(PickerMode::INDIVIDUAL_CELL);
    } else {
        timeSeriesMouseInteractor->deactivatePicker();
    }
}

void TimeSeriesChartDialog::on_btnClear_clicked() {
    QMessageBox::StandardButton button = QMessageBox::question(this, "Time series chart", "Are you sure?");
    
    if (button == QMessageBox::Yes) {
        TimeSeriesChartMouseInteractor *timeSeriesInteractor = TimeSeriesChartMouseInteractor::SafeDownCast(simulationVTKWidget->GetInteractor()->GetInteractorStyle());
        timeSeriesInteractor->removePickedCells(simulationVTKWidget->getLayerKey());
        chart->ClearPlots();
    }
}

void TimeSeriesChartDialog::on_btnPlot_clicked() {
    if (!isValid()) {
        return;
    }
    
    TimeSeriesChartMouseInteractor *timeSeriesInteractor = TimeSeriesChartMouseInteractor::SafeDownCast(simulationVTKWidget->GetInteractor()->GetInteractorStyle());
    vtkSmartPointer<vtkIdTypeArray> pickedCellsIds = timeSeriesInteractor->getCellIdArray(simulationVTKWidget->getLayerKey());
    
    if (!pickedCellsIds || pickedCellsIds->GetNumberOfTuples() == 0) {
        QMessageBox::warning(this, "Time Series Chart", "No picked cells.");
        return;
    }
    
    QByteArray layerNameByteArray = simulationVTKWidget->getLayerKey().split("-").first().toLocal8Bit();
    const char *layerName = layerNameByteArray.data();
    
    chart->ClearPlots();
    
    int i = 0;
    int initialFrame = ui->edtInitialFrame->text().toInt();
    int finalFrame = ui->edtFinalFrame->text().toInt();
    int frameStep = ui->edtFrameStep->text().toInt();
    int frame = initialFrame - 1;
    int framesNumerator = finalFrame - initialFrame + 1;
    int framesTotal = framesNumerator / frameStep + (framesNumerator % frameStep > 0 ? 1 : 0);
    
    vtkSmartPointer<vtkStringArray> timeStampStrings = vtkSmartPointer<vtkStringArray>::New();
    vtkSmartPointer<vtkDoubleArray> timeStamps = vtkSmartPointer<vtkDoubleArray>::New();
    timeStamps->SetNumberOfTuples(framesTotal);
    timeStamps->SetName("Time");
    
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
    table->AddColumn(timeStamps); // X axis
    
    QFileInfoList outputFiles = simulationVTKWidget->getCurrentSimulation()->getOutputFiles();

    while (i < framesTotal) {
        vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
        QByteArray fileNameByteArray = outputFiles.at(frame).absoluteFilePath().toLocal8Bit();
        reader->SetFileName(fileNameByteArray.data());
        reader->Update();
        
        std::string headerWithLineBreak = reader->GetHeader();
        const char *header = headerWithLineBreak.erase(headerWithLineBreak.length() - 1).c_str();
        QDateTime dateTime = QDateTime::fromString(header, Qt::ISODate);
        
        dateTime.setTimeSpec(Qt::UTC);
        table->SetValue(i, 0, dateTime.toTime_t());
        timeStampStrings->InsertNextValue(header);
        
        for (vtkIdType j = 0; j < pickedCellsIds->GetNumberOfTuples(); j++) {
            vtkIdType pickedCellId = pickedCellsIds->GetValue(j);
            
            if (j == table->GetNumberOfColumns() - 1) {
                vtkSmartPointer<vtkDoubleArray> cellCurve = vtkSmartPointer<vtkDoubleArray>::New();
                std::string cellCurveName = QString("Cell %1").arg(pickedCellId).toStdString();
                cellCurve->SetNumberOfTuples(framesTotal);
                cellCurve->SetName(cellCurveName.c_str());
                
                table->AddColumn(cellCurve);
            }
            
            table->SetValue(i, j + 1, reader->GetUnstructuredGridOutput()->GetCellData()->GetArray(layerName)->GetTuple1(pickedCellId));
        }
        
        frame += frameStep;
        i++;
    }
    
    for (vtkIdType i = 0; i < pickedCellsIds->GetNumberOfTuples(); i++) {
        double lineColor[3];
        timeSeriesInteractor->getCellColor(pickedCellsIds->GetValue(i), lineColor);
        
        vtkSmartPointer<vtkPlot> line = chart->AddPlot(vtkChart::LINE);
        line->GetPen()->SetColor(lineColor[0], lineColor[1], lineColor[2]);
        line->SetInputData(table, 0, i + 1);
        line->SetLabel(table->GetColumnName(i + 1));
        line->SetWidth(3.0);
    }
    
    vtkSmartPointer<vtkAxis> bottomAxis = chart->GetAxis(vtkAxis::BOTTOM);
    bottomAxis->SetTitle(timeStamps->GetName());
    bottomAxis->SetNumberOfTicks(5);
//    bottomAxis->SetRange(timeStamps->GetRange()[0], timeStamps->GetRange()[1]);
    bottomAxis->SetCustomTickPositions(timeStamps, timeStampStrings);
    
    vtkSmartPointer<vtkAxis> leftAxis = chart->GetAxis(vtkAxis::LEFT);
    leftAxis->SetTitle(layerName);
    
    ui->vtkWidget->GetRenderWindow()->Render();
}

void TimeSeriesChartDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);

    if (standardButton == QDialogButtonBox::Close) {
        TimeSeriesChartDialog::reject();
    }
}

void TimeSeriesChartDialog::btnExportToCSV_clicked() {
    if (chart->GetNumberOfPlots() == 0) {
        QMessageBox::warning(this, "Time Series Chart", "No plots available.");
        return;
    }
}

void TimeSeriesChartDialog::reject() {
    TimeSeriesChartMouseInteractor *timeSeriesInteractor = TimeSeriesChartMouseInteractor::SafeDownCast(simulationVTKWidget->GetInteractor()->GetInteractorStyle());
    
    timeSeriesInteractor->removePickedCells(simulationVTKWidget->getLayerKey());
    timeSeriesInteractor->deactivatePicker();
    
    QDialog::reject();
}

bool TimeSeriesChartDialog::isValid() {
    int finalFrame = ui->edtFinalFrame->text().toInt();
    int totalFrames = simulationVTKWidget->getCurrentSimulation()->getOutputFiles().size();
    
    if (finalFrame > totalFrames) {
        QMessageBox::warning(this, "Times Series Chart", QString("Final frame must be lesser or equal to %1.").arg(totalFrames));
        return false;
    }
    
    int initialFrame = ui->edtInitialFrame->text().toInt();
    
    if (initialFrame < 1 || finalFrame < 1 || initialFrame > finalFrame) {
        QMessageBox::warning(this, "Time Series Chart", "Invalid frame interval.");
        return false;
    }
    
    int frameStep = ui->edtFrameStep->text().toInt();
    
    if (frameStep >= finalFrame - initialFrame) {
        QMessageBox::warning(this, "Time Series Chart", QString("Frame step must be between %1 and %2.").arg(initialFrame).arg(finalFrame - 1));
        return false;
    }
    
    if (ui->chkImportShapefile->isChecked() && !QFile::exists(ui->edtShapefile->text())) {
        QMessageBox::warning(this, "Time Series Chart", "Shape file not found.");
        return false;
    }
    
    return true;
}

QString TimeSeriesChartDialog::getDefaultDirectory() {
    return appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(BOUNDARY_DEFAULT_DIR_KEY).toString();
}