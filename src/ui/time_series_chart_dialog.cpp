#include <ui/time_series_chart_dialog.h>
#include "ui_time_series_chart_dialog.h"

#include <utility/time_series_chart_mouse_interactor.h>

#include <vtkPen.h>
#include <QDateTime>
#include <vtkAxis.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <QToolButton>
#include <vtkCellData.h>
#include <vtkRenderer.h>
#include <vtkStringArray.h>
#include <vtkDoubleArray.h>
#include <vtkContextScene.h>
#include <vtkGenericDataObjectReader.h>

TimeSeriesChartDialog::TimeSeriesChartDialog(QWidget *parent, SimulationVTKWidget *simulationVTKWidget) :
	QDialog(parent),
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

    simulationVTKWidget->togglePicker(true);
    
    chart->SetShowLegend(true);
    view->GetScene()->AddItem(chart);
}

TimeSeriesChartDialog::~TimeSeriesChartDialog() {
    delete ui;
}

void TimeSeriesChartDialog::on_btnBrowseShapefile_clicked() {
    
}

void TimeSeriesChartDialog::on_btnPlot_clicked() {
    TimeSeriesChartMouseInteractor *timeSeriesInteractor = TimeSeriesChartMouseInteractor::SafeDownCast(simulationVTKWidget->GetInteractor()->GetInteractorStyle());
    vtkSmartPointer<vtkIdTypeArray> pickedCellsIds = timeSeriesInteractor->getCellIdArray(simulationVTKWidget->getLayerKey());
    QByteArray layerNameByteArray = simulationVTKWidget->getLayerKey().split("-").first().toLocal8Bit();
    const char *layerName = layerNameByteArray.data();
    
    QFileInfoList outputFiles = simulationVTKWidget->getCurrentSimulation()->getOutputFiles();
    int outputFilesSize = outputFiles.size();
    
    vtkSmartPointer<vtkStringArray> timeStampStrings = vtkSmartPointer<vtkStringArray>::New();
    vtkSmartPointer<vtkDoubleArray> timeStamps = vtkSmartPointer<vtkDoubleArray>::New();
    timeStamps->SetNumberOfTuples(outputFilesSize);
    timeStamps->SetName("Time");
    
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
    table->AddColumn(timeStamps); // X axis
    
    chart->ClearPlots();
    
    for (int i = 0; i < outputFilesSize; i++) {
        vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
        QByteArray fileNameByteArray = outputFiles.at(i).absoluteFilePath().toLocal8Bit();
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
                cellCurve->SetNumberOfTuples(outputFilesSize);
                cellCurve->SetName(cellCurveName.c_str());
                
                table->AddColumn(cellCurve);
            }
            
            table->SetValue(i, j + 1, reader->GetUnstructuredGridOutput()->GetCellData()->GetArray(layerName)->GetTuple1(pickedCellId));
        }
    }
    
    for (vtkIdType i = 0; i < pickedCellsIds->GetNumberOfTuples(); i++) {
        double lineColor[3];
        timeSeriesInteractor->getCellColor(pickedCellsIds->GetValue(i), lineColor);
        
        std::cout << pickedCellsIds->GetValue(i) << std::endl;
        
        vtkSmartPointer<vtkPlot> line = chart->AddPlot(vtkChart::LINE);
        line->GetPen()->SetColor(lineColor[0], lineColor[1], lineColor[2]);
        line->SetInputData(table, 0, i + 1);
        line->SetLabel(table->GetColumnName(i + 1));
        line->SetWidth(3.0);
    }
    
    vtkSmartPointer<vtkAxis> bottomAxis = chart->GetAxis(vtkAxis::BOTTOM);
    bottomAxis->SetNumberOfTicks(2);
    bottomAxis->SetCustomTickPositions(timeStamps, timeStampStrings);
    
    ui->vtkWidget->GetRenderWindow()->Render();
}

void TimeSeriesChartDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);

    if (standardButton == QDialogButtonBox::Close) {
        TimeSeriesChartDialog::reject();
    }
}

void TimeSeriesChartDialog::btnExportToCSV_clicked() {
    
}

void TimeSeriesChartDialog::reject() {
    simulationVTKWidget->togglePicker(false);
    QDialog::reject();
}