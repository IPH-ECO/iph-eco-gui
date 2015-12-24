#include <ui/time_series_chart_dialog.h>
#include "ui_time_series_chart_dialog.h"

#include <utility/time_series_chart_mouse_interactor.h>

#include <QDateTime>
#include <vtkAxis.h>
#include <vtkPlot.h>
#include <vtkTable.h>
#include <QToolButton>
#include <vtkChartXY.h>
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
    view(vtkSmartPointer<vtkContextView>::New())
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
}

TimeSeriesChartDialog::~TimeSeriesChartDialog() {
    delete ui;
}

void TimeSeriesChartDialog::on_btnBrowseShapefile_clicked() {
    
}

void TimeSeriesChartDialog::on_btnPlot_clicked() {
    vtkSmartPointer<vtkStringArray> timeStampStrings = vtkSmartPointer<vtkStringArray>::New();
//    timeStampStrings->SetName("Time");
    
    vtkSmartPointer<vtkDoubleArray> timeStamps = vtkSmartPointer<vtkDoubleArray>::New();
    timeStamps->SetName("Time");
    
    vtkSmartPointer<vtkDoubleArray> scalars = vtkSmartPointer<vtkDoubleArray>::New();
    scalars->SetName("Y Axis");
    
    TimeSeriesChartMouseInteractor *timeSeriesInteractor = TimeSeriesChartMouseInteractor::SafeDownCast(simulationVTKWidget->GetInteractor()->GetInteractorStyle());
    QString layerKey = simulationVTKWidget->getLayerKey();
    QByteArray layerNameByteArray = layerKey.split("-").first().toLocal8Bit();
    vtkIdType cellId = timeSeriesInteractor->getCellIdArray(simulationVTKWidget->getLayerKey())->GetValue(0);
    
    QFileInfoList outputFilesInfo = simulationVTKWidget->getCurrentSimulation()->getOutputFiles();
    
    for (QFileInfo outputFileInfo : outputFilesInfo) {
        QByteArray fileNameByteArray = outputFileInfo.absoluteFilePath().toLocal8Bit();
        vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
        reader->SetFileName(fileNameByteArray.data());
        reader->Update();
        
        uint timeStamp = QDateTime::fromString(reader->GetHeader(), Qt::ISODate).toTime_t();
        
        timeStamps->InsertNextValue(timeStamp);
        timeStampStrings->InsertNextValue(reader->GetHeader());
        
        vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid = reader->GetUnstructuredGridOutput();
        scalars->InsertNextValue(unstructuredGrid->GetCellData()->GetArray(layerNameByteArray.data())->GetTuple1(cellId));
    }
    
    vtkSmartPointer<vtkTable> table = vtkSmartPointer<vtkTable>::New();
    table->AddColumn(timeStamps);
    table->AddColumn(scalars);
    
    vtkSmartPointer<vtkChartXY> chart = vtkSmartPointer<vtkChartXY>::New();
    view->GetScene()->AddItem(chart);
    
    vtkSmartPointer<vtkPlot> line = chart->AddPlot(vtkChart::LINE);
    line->SetInputData(table, 0, 1);
    line->SetColor(0, 255, 0, 255);
    line->SetWidth(1.0);
    
    vtkSmartPointer<vtkAxis> bottomAxis = chart->GetAxis(vtkAxis::BOTTOM);
//    bottomAxis->SetCustomTickPositions(timeStamps, timeStampStrings);
    bottomAxis->SetNumberOfTicks(2);
//    bottomAxis->SetBehavior(vtkAxis::FIXED);
    
    ui->vtkWidget->GetRenderWindow()->Render();
}

void TimeSeriesChartDialog::on_buttonBox_clicked(QAbstractButton *button) {
    QDialogButtonBox::StandardButton standardButton = ui->buttonBox->standardButton(button);

    if (standardButton == QDialogButtonBox::Cancel) {
        TimeSeriesChartDialog::reject();
    }
}

void TimeSeriesChartDialog::btnExportToCSV_clicked() {
    
}

void TimeSeriesChartDialog::reject() {
    simulationVTKWidget->togglePicker(false);
    QDialog::reject();
}