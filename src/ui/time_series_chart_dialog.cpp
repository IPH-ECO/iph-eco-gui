#include <ui/time_series_chart_dialog.h>
#include "ui_time_series_chart_dialog.h"

#include <ui/coordinate_file_dialog.h>
#include <ui/layer_properties_dialog.h>
#include <utility/time_series_chart_2d_interactor_style.h>

#include <QFile>
#include <vtkPen.h>
#include <QDateTime>
#include <vtkAxis.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <QTextStream>
#include <QMessageBox>
#include <QToolButton>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkPointData.h>
#include <vtkPNGWriter.h>
#include <QProgressDialog>
#include <vtkDoubleArray.h>
#include <vtkTextProperty.h>
#include <vtkContextScene.h>
#include <vtkScalarBarActor.h>
#include <vtkArrayCalculator.h>
#include <vtkSimplePointsReader.h>
#include <vtkWindowToImageFilter.h>
#include <vtkGenericDataObjectReader.h>

vtkStandardNewMacro(TimeSeriesChart2DInteractorStyle);

TimeSeriesChartDialog::TimeSeriesChartDialog(QWidget *parent, SimulationVTKWidget *simulationVTKWidget) :
	QDialog(parent),
    TIME_SERIES_CHART_DIR_KEY("time_series_chart_dir"),
	ui(new Ui::TimeSeriesChartDialog),
    simulationVTKWidget(simulationVTKWidget),
    renderer(vtkSmartPointer<vtkRenderer>::New()),
    view(vtkSmartPointer<vtkContextView>::New()),
    chart(vtkSmartPointer<vtkChartXY>::New()),
    layerProperties(new LayerProperties()),
    appSettings(new QSettings(QApplication::organizationName(), QApplication::applicationName(), this))
{
    this->timeSeriesInteractor = TimeSeriesChartMouseInteractor::SafeDownCast(simulationVTKWidget->GetInteractor()->GetInteractorStyle());
    Simulation *simulation = simulationVTKWidget->getCurrentSimulation();
    
    ui->setupUi(this);
    ui->vtkVerticalProfileWidget->hide();
    ui->edtInitialFrame->setText("1");
    ui->edtFrameStep->setText("1");
    ui->edtFinalFrame->setText(QString::number(simulation->getOutputFiles().size()));
    this->resize(this->minimumSize());
    
    for (int i = simulationVTKWidget->getNumberOfMapLayers() - 1; i >= 0; i--) {
        ui->cbxLayer->addItem(QString::number(i + 1));
    }
    
    if (simulationVTKWidget->getNumberOfMapLayers() == 1) {
        ui->chkVerticalProfile->setDisabled(true);
    }
    
    QToolButton *btnLayerProperties = new QToolButton(this);
    btnLayerProperties->setIcon(QIcon(":/icons/layer-properties.png"));
    btnLayerProperties->setToolTip("Change chart properties");
    btnLayerProperties->setObjectName("btnLayerProperties");
    btnLayerProperties->setDisabled(true);
    ui->buttonBox->addButton(btnLayerProperties, QDialogButtonBox::ActionRole);
    connect(btnLayerProperties, SIGNAL(clicked()), this, SLOT(btnLayerProperties_clicked()));
    
    QToolButton *btnExportToCSV = new QToolButton(this);
    btnExportToCSV->setIcon(QIcon(":/icons/text-csv.png"));
    btnExportToCSV->setToolTip("Export chart data to CSV");
    ui->buttonBox->addButton(btnExportToCSV, QDialogButtonBox::ActionRole);
    connect(btnExportToCSV, SIGNAL(clicked()), this, SLOT(btnExportToCSV_clicked()));
    
    QToolButton *btnExportToPNG = new QToolButton(this);
    btnExportToPNG->setIcon(QIcon(":/icons/image-x-generic.png"));
    btnExportToPNG->setToolTip("Export chart to PNG");
    ui->buttonBox->addButton(btnExportToPNG, QDialogButtonBox::ActionRole);
    connect(btnExportToPNG, SIGNAL(clicked()), this, SLOT(btnExportToPNG_clicked()));

    renderer->SetBackground(1.0, 1.0, 1.0);
    ui->vtkVerticalProfileWidget->GetRenderWindow()->AddRenderer(renderer);
    ui->vtkVerticalProfileWidget->GetInteractor()->SetInteractorStyle(vtkSmartPointer<TimeSeriesChart2DInteractorStyle>::New());
    
    view->SetInteractor(ui->vtkChartWidget->GetInteractor());
    view->GetRenderer()->SetBackground(1.0, 1.0, 1.0);
    view->GetScene()->AddItem(chart);
    ui->vtkChartWidget->SetRenderWindow(view->GetRenderWindow());
    chart->SetShowLegend(true);
}

TimeSeriesChartDialog::~TimeSeriesChartDialog() {
    delete layerProperties;
    delete appSettings;
    delete ui;
}

void TimeSeriesChartDialog::on_btnBrowsePointsFile_clicked() {
    QString extensions = "Text file (*.txt *xyz);;Keyhole Markup Language file (*.kml)";
    CoordinateFileDialog *dialog = new CoordinateFileDialog(this, tr("Select a coordinate file"), getDefaultDirectory(), extensions);
    int exitCode = dialog->exec();
    
    if (exitCode == QDialog::Accepted) {
        ui->edtPointsFile->setText(dialog->selectedFiles().first());
    }
}

void TimeSeriesChartDialog::on_btnPicker_toggled(bool checked) {
    if (checked) {
        timeSeriesInteractor->activatePicker(ui->chkVerticalProfile->isChecked() ? PickerMode::ONE_CELL : PickerMode::EACH_CELL);
    } else {
        timeSeriesInteractor->deactivatePicker();
    }
}

void TimeSeriesChartDialog::on_btnClear_clicked() {
    QMessageBox::StandardButton button = QMessageBox::question(this, "Time series chart", "Are you sure?");
    
    if (button == QMessageBox::Yes) {
        timeSeriesInteractor->removePickedCells(simulationVTKWidget->getLayerKey());
        chart->ClearPlots();
    }
}

void TimeSeriesChartDialog::on_btnPlot_clicked() {
    if (!isValid()) {
        return;
    }
    
    vtkSmartPointer<vtkIdTypeArray> pickedCellsIds = getCellsIds();
    
    if (!pickedCellsIds || pickedCellsIds->GetNumberOfTuples() == 0) {
        QMessageBox::warning(this, "Time Series Chart", "No picked cells.");
        return;
    }
    
    QByteArray layerNameByteArray = simulationVTKWidget->getLayerKey().split("-").first().toLocal8Bit();
    const char *layerName = layerNameByteArray.data();
    bool verticalProfile = ui->chkVerticalProfile->isChecked();
    
    int initialFrame = ui->edtInitialFrame->text().toInt();
    int finalFrame = ui->edtFinalFrame->text().toInt();
    int frameStep = ui->edtFrameStep->text().toInt();
    int frame = initialFrame - 1;
    int framesNumerator = finalFrame - initialFrame + 1;
    int framesTotal = framesNumerator / frameStep + (framesNumerator % frameStep > 0 ? 1 : 0);
    
    vtkSmartPointer<vtkStringArray> timeStampStrings = vtkSmartPointer<vtkStringArray>::New();
    vtkSmartPointer<vtkDoubleArray> timeStamps;
    vtkSmartPointer<vtkTable> table;
    
    if (!verticalProfile) {
        timeStamps = vtkSmartPointer<vtkDoubleArray>::New();
        timeStamps->SetNumberOfTuples(framesTotal);
        timeStamps->SetName("Time");
        
        table = vtkSmartPointer<vtkTable>::New();
        table->AddColumn(timeStamps); // X axis
        
        chart->ClearPlots();
    }
    
    QFileInfoList outputFiles = simulationVTKWidget->getCurrentSimulation()->getOutputFiles();
    vtkSmartPointer<vtkDoubleArray> verticalProfileScalars;
    vtkSmartPointer<vtkDoubleArray> xCoordinates;
    vtkSmartPointer<vtkDoubleArray> yCoordinates;
    
    if (verticalProfile) {
        verticalProfileScalars = vtkSmartPointer<vtkDoubleArray>::New();
        xCoordinates = vtkSmartPointer<vtkDoubleArray>::New();
        yCoordinates = vtkSmartPointer<vtkDoubleArray>::New();
    }
    
    QProgressDialog *progressDialog = new QProgressDialog(this);
    progressDialog->setMinimum(0);
    progressDialog->setMaximum(framesTotal);
    progressDialog->setMinimumDuration(500);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setLabelText("Plotting...");
    
    for (int i = 0; i < framesTotal; i++) {
        vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
        QByteArray fileNameByteArray = outputFiles.at(frame).absoluteFilePath().toLocal8Bit();
        reader->SetFileName(fileNameByteArray.data());
        reader->Update();
        
        vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = reader->GetUnstructuredGridOutput();
        vtkSmartPointer<vtkDoubleArray> cellDataArray = getGridArray(unstructuredGrid, layerName);
        
        std::string headerWithLineBreak = reader->GetHeader();
        const char *header = headerWithLineBreak.erase(headerWithLineBreak.length() - 1).c_str(); // removes new line character
        timeStampStrings->InsertNextValue(header);
        
        if (verticalProfile) {
            xCoordinates->InsertNextTuple1(i / 2.0);
            
            for (int l = simulationVTKWidget->getNumberOfMapLayers() - 1; l >= 0; l--) {
                vtkIdType cellId = getCorrespondingCell(pickedCellsIds->GetValue(0), l + 1);
                
                if (yCoordinates->GetNumberOfTuples() < simulationVTKWidget->getNumberOfMapLayers()) {
                    double bounds[6];
                    unstructuredGrid->GetCell(cellId)->GetBounds(bounds);
                    yCoordinates->InsertNextTuple1(bounds[4]);
                }

                verticalProfileScalars->InsertNextTuple1(cellDataArray->GetTuple1(cellId));
            }
        } else {
            table->SetValue(i, 0, i);
            
            for (vtkIdType j = 0; j < pickedCellsIds->GetNumberOfTuples(); j++) {
                vtkIdType cellId = getCorrespondingCell(pickedCellsIds->GetValue(j), ui->cbxLayer->currentText().toInt());
                
                if (j == table->GetNumberOfColumns() - 1) {
                    vtkSmartPointer<vtkDoubleArray> cellCurve = vtkSmartPointer<vtkDoubleArray>::New();
                    QByteArray cellCurveName = QString("Cell %1").arg(cellId).toLocal8Bit();
                    cellCurve->SetNumberOfTuples(framesTotal);
                    cellCurve->SetName(cellCurveName.constData());
                    
                    table->AddColumn(cellCurve);
                }
                
                table->SetValue(i, j + 1, cellDataArray->GetTuple1(cellId));
            }
        }
        
        if (progressDialog->wasCanceled()) {
            progressDialog->deleteLater();
            return;
        }
        
        frame += frameStep;
        progressDialog->setValue(i + 1);
        QApplication::processEvents();
    }
    
    progressDialog->deleteLater();
    
    if (verticalProfile) {
        renderer->RemoveAllViewProps();
        renderVerticalProfileGrid(layerName, xCoordinates, yCoordinates, verticalProfileScalars);
        renderVerticalProfileAxes(xCoordinates->GetRange(), yCoordinates->GetRange(), timeStampStrings);
        renderer->ResetCamera();
        ui->vtkChartWidget->hide();
        ui->vtkVerticalProfileWidget->show();
        ui->vtkVerticalProfileWidget->GetRenderWindow()->Render();
    } else {
        for (vtkIdType i = 0; i < pickedCellsIds->GetNumberOfTuples(); i++) {
            double lineColor[3];
            timeSeriesInteractor->getCellColor(pickedCellsIds->GetValue(i), lineColor);
            
            vtkSmartPointer<vtkPlot> line = chart->AddPlot(vtkChart::LINE);
            line->SetInputData(table, 0, i + 1);
            line->SetIndexedLabels(timeStampStrings);
            line->SetLabel(table->GetColumnName(i + 1));
            line->GetPen()->SetColor(lineColor[0], lineColor[1], lineColor[2]);
            line->SetWidth(2.0);
        }
        
        int numberOfTicks = 6;
        vtkSmartPointer<vtkDoubleArray> labelPositions = vtkSmartPointer<vtkDoubleArray>::New();
        vtkSmartPointer<vtkStringArray> xLabels = vtkSmartPointer<vtkStringArray>::New();
        int tickIncrement = table->GetNumberOfRows() / (numberOfTicks - 1);
        
        for (vtkIdType i = 0; i < table->GetNumberOfRows(); i++) {
            if (labelPositions->GetNumberOfTuples() != numberOfTicks && (i % tickIncrement == 0 || i == timeStamps->GetNumberOfTuples() - 1)) {
                labelPositions->InsertNextValue(timeStamps->GetValue(i));
                xLabels->InsertNextValue(timeStampStrings->GetValue(i));
            }
        }
        
        vtkSmartPointer<vtkAxis> bottomAxis = chart->GetAxis(vtkAxis::BOTTOM);
        bottomAxis->SetTitle(timeStamps->GetName());
        bottomAxis->SetCustomTickPositions(labelPositions, xLabels);
        
        QString component = simulationVTKWidget->getLayerKey().split("-").last();
        QByteArray axisLabelByteArray = (QString(layerName) + (component.isEmpty() ? "" : QString(" (%1)").arg(component))).toLocal8Bit();
        const char *axisLabel = axisLabelByteArray.data();
        vtkSmartPointer<vtkAxis> leftAxis = chart->GetAxis(vtkAxis::LEFT);
        leftAxis->SetTitle(axisLabel);
        
        ui->vtkVerticalProfileWidget->hide();
        ui->vtkChartWidget->show();
    }
}

void TimeSeriesChartDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);

    if (standardButton == QDialogButtonBox::Close) {
        TimeSeriesChartDialog::reject();
    }
}

void TimeSeriesChartDialog::btnExportToCSV_clicked() {
    if ((!ui->chkVerticalProfile->isChecked() && chart->GetNumberOfPlots() == 0) || (ui->chkVerticalProfile->isChecked() && !rectilinearGrid)) {
        QMessageBox::warning(this, "Time Series Chart", "No plots available.");
        return;
    }
    
    QString csvFileName = QFileDialog::getSaveFileName(this, "Time Series Chart", getDefaultDirectory(), "Comma Separated Value (*.csv)");
    
    if (!csvFileName.isEmpty()) {
        QFile csvFile(csvFileName);
        
        if (!csvFile.open(QFile::WriteOnly | QFile::Truncate)) {
            QMessageBox::warning(this, "Time Series Chart", QString("Unable to write to file %1").arg(csvFileName));
            csvFile.close();
            return;
        }
        
        QTextStream outputStream(&csvFile);
        QStringList fileLines;
        
        if (ui->chkVerticalProfile->isChecked()) {
            fileLines << "\"Time\"";
            fileLines << "\"Elevation\"";
            fileLines << "\"Z\"";
            
            outputStream << fileLines.join(";");
            outputStream << "\r\n";
            
            fileLines.clear();
            
            vtkSmartPointer<vtkStringArray> timeStampsStrings = this->axesActor->GetAxisLabels(0);
            vtkIdType count = 0;
            
            for (vtkIdType i = 0; i < timeStampsStrings->GetNumberOfTuples(); i++) {
                vtkSmartPointer<vtkDataArray> yCoordinates = this->rectilinearGrid->GetYCoordinates();
                
                for (vtkIdType j = 0; j < yCoordinates->GetNumberOfTuples(); j++) {
                    fileLines << QString("\"%1\"").arg(timeStampsStrings->GetValue(i).c_str());
                    fileLines << QString("\"%1\"").arg(yCoordinates->GetTuple1(j));
                    fileLines << QString("\"%1\"").arg(rectilinearGrid->GetPointData()->GetScalars()->GetTuple1(count));

                    outputStream << fileLines.join(";");
                    outputStream << "\r\n";
                    outputStream.flush();
                    
                    fileLines.clear();
                    count++;
                }
            }
        } else {
            fileLines << "\"Layer\"";
            fileLines << "\"Time\"";
            
            for (vtkIdType p = 0; p < chart->GetNumberOfPlots(); p++) {
                fileLines << QString("\"%1\"").arg(chart->GetPlot(p)->GetLabel().c_str());
            }
            
            outputStream << fileLines.join(";");
            outputStream << "\r\n";
            
            fileLines.clear();
            
            for (vtkIdType p = 0; p < chart->GetNumberOfPlots(); p++) {
                vtkSmartPointer<vtkPlot> plot = chart->GetPlot(p);
                vtkSmartPointer<vtkTable> table = plot->GetInput();
                vtkSmartPointer<vtkStringArray> timeStampsStrings = plot->GetIndexedLabels();
                
                for (vtkIdType i = 0; i < table->GetNumberOfRows(); i++) {
                    fileLines << QString("\"%1\"").arg(ui->cbxLayer->currentText());
                    fileLines << timeStampsStrings->GetValue(i).c_str();
                
                    for (vtkIdType j = 1; j < table->GetNumberOfColumns(); j++) {
                        fileLines << QString("\"%1\"").arg(QString::number(table->GetValue(i, j).ToDouble(), 'f', 6));
                    }
                    
                    outputStream << fileLines.join(";");
                    outputStream << "\r\n";
                    outputStream.flush();
                    
                    fileLines.clear();
                }
            }
        }
        
        csvFile.close();
        QMessageBox::information(this, "Time Series Chart", "CSV file exported.");
    }
}

void TimeSeriesChartDialog::reject() {
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
    
    if (ui->chkImportPointsFile->isChecked() && !QFile::exists(ui->edtPointsFile->text())) {
        QMessageBox::warning(this, "Time Series Chart", "Points file not found.");
        return false;
    }
    
    return true;
}

QString TimeSeriesChartDialog::getDefaultDirectory() {
    return appSettings->value(TIME_SERIES_CHART_DIR_KEY).toString().isEmpty() ? QDir::homePath() : appSettings->value(TIME_SERIES_CHART_DIR_KEY).toString();
}

vtkSmartPointer<vtkIdTypeArray> TimeSeriesChartDialog::getCellsIds() const {
    if (ui->chkImportPointsFile->isChecked()) {
        vtkSmartPointer<vtkGenericDataObjectReader> genericObjectReader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
        QByteArray fileNameByteArray = simulationVTKWidget->getCurrentSimulation()->getOutputFiles().first().absoluteFilePath().toLocal8Bit();
        genericObjectReader->SetFileName(fileNameByteArray.data());
        genericObjectReader->Update();
        
        vtkSmartPointer<vtkSimplePointsReader> pointsReader = vtkSmartPointer<vtkSimplePointsReader>::New();
        fileNameByteArray = ui->edtPointsFile->text().toLocal8Bit();
        pointsReader->SetFileName(fileNameByteArray.data());
        pointsReader->Update();
        
        vtkSmartPointer<vtkIdTypeArray> cellsIds = vtkSmartPointer<vtkIdTypeArray>::New();
        cellsIds->SetNumberOfComponents(1);
        
        vtkSmartPointer<vtkUnstructuredGrid> firstFrameGrid = genericObjectReader->GetUnstructuredGridOutput();
        vtkSmartPointer<vtkPoints> points = pointsReader->GetOutput()->GetPoints();
        double pcoords[3] = {0, 0, 0};
        double weights[8];
        int subId;
        
        for (vtkIdType i = 0; i < points->GetNumberOfPoints(); i++) {
            vtkIdType cellId = firstFrameGrid->FindCell(points->GetPoint(i), nullptr, 0, 0.001, subId, pcoords, weights);

            if (cellId != -1) {
                cellsIds->InsertNextTuple1(cellId);
            }
        }
        
        timeSeriesInteractor->renderCellsIds(simulationVTKWidget->getLayerKey(), firstFrameGrid, cellsIds);
    }
    
    return timeSeriesInteractor->getCellIdArray(simulationVTKWidget->getLayerKey());
}

void TimeSeriesChartDialog::btnExportToPNG_clicked() {
    if ((!ui->chkVerticalProfile->isChecked() && chart->GetNumberOfPlots() == 0) || (ui->chkVerticalProfile->isChecked() && !rectilinearGrid)) {
        QMessageBox::warning(this, "Time Series Chart", "No plots available.");
        return;
    }
    
    QString imageFileName = QFileDialog::getSaveFileName(this, "Time Series Chart", getDefaultDirectory(), "Portable Graphics Network (*.png)");
    
    if (!imageFileName.isEmpty()) {
        vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter = vtkSmartPointer<vtkWindowToImageFilter>::New();
        windowToImageFilter->SetInput(ui->chkVerticalProfile->isChecked() ? ui->vtkVerticalProfileWidget->GetRenderWindow() : ui->vtkChartWidget->GetRenderWindow());
        windowToImageFilter->SetMagnification(1); //set the resolution of the output image (3 times the current resolution of vtk render window)
        windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
        windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
        windowToImageFilter->Update();
        
        QByteArray imageFileNameByteArray = imageFileName.toLocal8Bit();
        vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New();
        writer->SetFileName(imageFileNameByteArray.data());
        writer->SetInputConnection(windowToImageFilter->GetOutputPort());
        writer->Write();
        
        QMessageBox::information(this, "Time Series Chart", "Chart exported.");
    }
}

vtkIdType TimeSeriesChartDialog::getCorrespondingCell(const vtkIdType &sourceCellId, const int &layer) const {
    vtkIdType numberOfCells = simulationVTKWidget->getCurrentSimulation()->getMesh()->getMeshPolyData()->GetNumberOfCells();
    vtkIdType sourceLayer = sourceCellId / numberOfCells;
    vtkIdType sourceLayerFirstCellId = sourceLayer * numberOfCells;
    vtkIdType cellOffset = sourceCellId - sourceLayerFirstCellId;
    vtkIdType targetLayer = layer - 1;
    vtkIdType targetLayerFirstCellId = targetLayer * numberOfCells;
    
    return targetLayerFirstCellId + cellOffset;
}

void TimeSeriesChartDialog::on_chkVerticalProfile_toggled(bool checked) {
    QToolButton *btnLayerProperties = ui->buttonBox->findChild<QToolButton*>("btnLayerProperties");
    
    if (checked) {
        vtkSmartPointer<vtkIdTypeArray> pickedCellsIds = getCellsIds();
        bool proceed = true;
        
        if (pickedCellsIds && pickedCellsIds->GetNumberOfTuples() > 0) {
            QString question = "This action will discard the current selected cells. Would you like to proceed?";
            QMessageBox::StandardButton button = QMessageBox::question(this, "Time Series Chart", question);
            proceed = button == QMessageBox::Yes;
        }
        
        btnLayerProperties->setEnabled(proceed);
        
        if (proceed) {
            chart->ClearPlots();
            timeSeriesInteractor->removePickedCells(simulationVTKWidget->getLayerKey());
            timeSeriesInteractor->activatePicker(checked ? PickerMode::ONE_CELL : PickerMode::EACH_CELL);
        } else {
            ui->chkVerticalProfile->setChecked(false);
        }
    } else {
        btnLayerProperties->setEnabled(false);
        timeSeriesInteractor->activatePicker(PickerMode::EACH_CELL);
        renderer->RemoveAllViewProps();
        ui->vtkVerticalProfileWidget->GetRenderWindow()->Render();
    }
}

void TimeSeriesChartDialog::renderVerticalProfileGrid(const char *layerName, vtkSmartPointer<vtkDoubleArray> x, vtkSmartPointer<vtkDoubleArray> y, vtkSmartPointer<vtkDoubleArray> scalars) {
    
    vtkSmartPointer<vtkDoubleArray> zCoordinates = vtkSmartPointer<vtkDoubleArray>::New();
    zCoordinates->InsertNextValue(0.0);
    
    rectilinearGrid = vtkSmartPointer<vtkRectilinearGrid>::New();
    rectilinearGrid->SetDimensions(x->GetNumberOfTuples(), y->GetNumberOfTuples(), 1);
    rectilinearGrid->SetXCoordinates(x);
    rectilinearGrid->SetYCoordinates(y);
    rectilinearGrid->SetZCoordinates(zCoordinates);
    rectilinearGrid->GetPointData()->SetScalars(scalars);
    
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = buildColorTransferFunction(scalars->GetRange());
    vtkSmartPointer<vtkScalarBarActor> scalarBarActor = vtkSmartPointer<vtkScalarBarActor>::New();
    scalarBarActor->SetLookupTable(colorTransferFunction);
    scalarBarActor->SetNumberOfLabels(4);
    scalarBarActor->SetPosition(.01, .68);
    scalarBarActor->SetHeight(.3);
    scalarBarActor->SetWidth(.14);
    
    vtkSmartPointer<vtkDataSetMapper> rectilinearGridMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    rectilinearGridMapper->SetInputData(rectilinearGrid);
    rectilinearGridMapper->SetScalarModeToUsePointData();
    rectilinearGridMapper->SetLookupTable(scalarBarActor->GetLookupTable());
    rectilinearGridMapper->UseLookupTableScalarRangeOn();
    rectilinearGridMapper->ScalarVisibilityOn();
    
    vtkSmartPointer<vtkActor> verticalProfileActor = vtkSmartPointer<vtkActor>::New();
    verticalProfileActor->SetMapper(rectilinearGridMapper);
    
    renderer->AddActor(verticalProfileActor);
    renderer->AddActor(scalarBarActor);
}

void TimeSeriesChartDialog::renderVerticalProfileAxes(double *xRange, double *yRange, vtkSmartPointer<vtkStringArray> timeStamps) {
    axesActor = vtkSmartPointer<vtkCubeAxesActor>::New();
    axesActor->SetXTitle("Time");
    axesActor->SetYUnits("m");
    axesActor->SetYTitle("Elevation");
    axesActor->GetLabelTextProperty(0)->SetOrientation(45);
    axesActor->GetLabelTextProperty(1)->SetOrientation(90);
    axesActor->XAxisMinorTickVisibilityOff();
    axesActor->YAxisMinorTickVisibilityOff();
    axesActor->GetLabelTextProperty(0)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(0)->SetColor(0, 0, 0);
    axesActor->GetXAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->GetLabelTextProperty(1)->SetColor(0, 0, 0);
    axesActor->GetTitleTextProperty(1)->SetColor(0, 0, 0);
    axesActor->GetYAxesLinesProperty()->SetColor(0, 0, 0);
    axesActor->SetBounds(xRange[0], xRange[1], yRange[0], yRange[1], 0, 0);
    axesActor->SetCamera(renderer->GetActiveCamera());
    axesActor->SetAxisLabels(0, timeStamps);
    axesActor->SetLabelOffset(10);
    axesActor->ZAxisVisibilityOff();
    
    renderer->AddActor(axesActor);
}

vtkSmartPointer<vtkColorTransferFunction> TimeSeriesChartDialog::buildColorTransferFunction(double *scalarBarRange) {
    QList<QColor> colors = ColorGradientTemplate::getColors(layerProperties->getMapColorGradient());
    bool invertScalarBar = layerProperties->getMapInvertColorGradient();
    double minimumRange = layerProperties->getUseCustomMapMinimum() ? layerProperties->getCustomMapMininumRange() : scalarBarRange[0];
    double maximumRange = layerProperties->getUseCustomMapMaximum() ? layerProperties->getCustomMapMaximumRange() : scalarBarRange[1];
    
    vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
    double interval = maximumRange - minimumRange;
    
    if (invertScalarBar) {
        for (int i = colors.size() - 1, j = 0; i > 0; i--, j++) {
            double x = minimumRange + j * interval / (double) colors.size();
            colorTransferFunction->AddRGBPoint(x, colors[i].redF(), colors[i].greenF(), colors[i].blueF());
        }
        colorTransferFunction->AddRGBPoint(maximumRange, colors.first().redF(), colors.first().greenF(), colors.first().blueF());
    } else {
        for (int i = 0; i < colors.size() - 1; i++) {
            double x = minimumRange + i * interval / (double) colors.size();
            colorTransferFunction->AddRGBPoint(x, colors[i].redF(), colors[i].greenF(), colors[i].blueF());
        }
        colorTransferFunction->AddRGBPoint(maximumRange, colors.last().redF(), colors.last().greenF(), colors.last().blueF());
    }
    
    return colorTransferFunction;
}

vtkSmartPointer<vtkDoubleArray> TimeSeriesChartDialog::getGridArray(vtkSmartPointer<vtkUnstructuredGrid> sourceGrid, const char *layerName) const {
    if (sourceGrid->GetCellData()->GetArray(layerName)->GetNumberOfComponents() == 1) {
        return vtkDoubleArray::SafeDownCast(sourceGrid->GetCellData()->GetArray(layerName));
    }
    
    vtkSmartPointer<vtkArrayCalculator> magnitudeFunction = vtkSmartPointer<vtkArrayCalculator>::New();
    const char *magnitudeArrayName = "magnitude";
    
    magnitudeFunction->AddScalarVariable("x", layerName, 0);
    magnitudeFunction->AddScalarVariable("y", layerName, 1);
    magnitudeFunction->AddScalarVariable("z", layerName, 2);
    magnitudeFunction->SetResultArrayName(magnitudeArrayName);
    magnitudeFunction->SetFunction("sqrt(x^2 + y^2 + z^2)");
    magnitudeFunction->SetAttributeModeToUseCellData();
    magnitudeFunction->SetInputData(sourceGrid);
    magnitudeFunction->Update();
    
    vtkSmartPointer<vtkUnstructuredGrid> magnitudeGrid = magnitudeFunction->GetUnstructuredGridOutput();
    
    return vtkSmartPointer<vtkDoubleArray>(vtkDoubleArray::SafeDownCast(magnitudeGrid->GetCellData()->GetArray(magnitudeArrayName)));
}

void TimeSeriesChartDialog::btnLayerProperties_clicked() {
    LayerPropertiesDialog *dialog = new LayerPropertiesDialog(this, layerProperties, LayerPropertiesDialog::MAP);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    QObject::connect(dialog, SIGNAL(applyChanges()), this, SLOT(on_btnPlot_clicked()));
    dialog->exec();
}