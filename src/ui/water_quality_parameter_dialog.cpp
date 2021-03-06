#include <ui/water_quality_parameter_dialog.h>
#include <ui_water_quality_parameter_dialog.h>

WaterQualityParameterDialog::WaterQualityParameterDialog(QWidget *parent, WaterQualityParameter *parameter, const QList<int> &distribuition) :
	QDialog(parent),
	ui(new Ui::WaterQualityParameterDialog()),
    parameter(parameter),
    waterQualityRepository(WaterQualityRepository::getInstance())
{
	ui->setupUi(this);

    QFont font = ui->lblParameter->font();
    font.setBold(true);
    ui->lblParameter->setFont(font);
    ui->lblParameter->setText(parameter->getName());
    
    QStringList horizontalLabels;
    QStringList verticalLabels;
    QStringList groups = parameter->getGroups();
    
    for (int i = 0; i < groups.size(); i++) {
        int rows = distribuition[i];
        
        WaterQualityParameter *groupParameter = waterQualityRepository->findParameterByName(groups[i], WaterQualityParameterSection::PARAMETER);
        
        horizontalLabels << QString("%1 (%2)").arg(groupParameter->getLabel()).arg(distribuition[i]);
        ui->trwParameters->insertColumn(i);
        
        if (rows > ui->trwParameters->rowCount()) {
            ui->trwParameters->setRowCount(rows);
        }
    }
    
    for (int i = 0; i < ui->trwParameters->rowCount(); i++) {
        verticalLabels << QString("Group %1").arg(i + 1);
        
        for (int j = 0; j < ui->trwParameters->columnCount(); j++) {
            QTableWidgetItem *tableItem = new QTableWidgetItem();
            
            if (i + 1 > distribuition[j]) {
                tableItem->setFlags(Qt::NoItemFlags);
                tableItem->setBackgroundColor(QColor(Qt::lightGray));
            } else {
                tableItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
                tableItem->setText(QString::number(parameter->getGroupValues().at(j)));
            }
            
            ui->trwParameters->setItem(i, j, tableItem);
        }
    }
    
    ui->trwParameters->setHorizontalHeaderLabels(horizontalLabels);
    ui->trwParameters->setVerticalHeaderLabels(verticalLabels);
    ui->trwParameters->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

WaterQualityParameterDialog::~WaterQualityParameterDialog() {
    delete ui;
}

void WaterQualityParameterDialog::accept() {
    QList<double> groupValues;
    
    for (int i = 0; i < ui->trwParameters->rowCount(); i++) {
        for (int j = 0; j < ui->trwParameters->columnCount(); j++) {
            QTableWidgetItem *tableItem = ui->trwParameters->item(i, j);
            
            if (tableItem->flags() == Qt::NoItemFlags) {
                continue;
            }

            groupValues << tableItem->text().toDouble();
        }
    }
    
    parameter->setGroupValues(groupValues);
    
    QDialog::accept();
}