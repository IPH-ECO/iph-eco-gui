#include <ui/water_quality_parameter_dialog.h>
#include <ui_water_quality_parameter_dialog.h>

WaterQualityParameterDialog::WaterQualityParameterDialog(QWidget *parent, WaterQualityConfiguration *configuration, WaterQualityParameter *parameter, const QList<int> &distribuition) :
	QDialog(parent),
	ui(new Ui::WaterQualityParameterDialog()),
    configuration(configuration),
    parameter(parameter)
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
        WaterQualityParameter *groupParameter = configuration->getParameter(groups[i], WaterQualityParameterSection::PARAMETER);
        int rows = distribuition[i];
        
        horizontalLabels << QString("%1 (%2)").arg(groupParameter->getLabel()).arg(distribuition[i]);
        ui->trwParameters->insertColumn(i);
        
        if (rows > ui->trwParameters->rowCount()) {
            ui->trwParameters->setRowCount(rows);
        }
    }
    
    QMap<QString, QList<double> > groupValues = parameter->getGroupValues();
    
    for (int i = 0; i < ui->trwParameters->rowCount(); i++) {
        verticalLabels << QString("Group %1").arg(i + 1);
        
        for (int j = 0; j < ui->trwParameters->columnCount(); j++) {
            QTableWidgetItem *tableItem = new QTableWidgetItem();
            
            if (i + 1 > distribuition[j]) {
                tableItem->setFlags(Qt::NoItemFlags);
                tableItem->setBackgroundColor(QColor(Qt::lightGray));
            } else {
                QList<double> values = groupValues.value(groups.at(j));
                
                tableItem->setText(QString::number(i < values.size() ? values.at(i) : parameter->getDefaultGroupValues().value(groups.at(j))));
                tableItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
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
    QMap<QString, QList<double> > groupValues;
    
    for (int j = 0; j < ui->trwParameters->columnCount(); j++) {
        QList<double> values;
        
        for (int i = 0; i < ui->trwParameters->rowCount(); i++) {
            QTableWidgetItem *tableItem = ui->trwParameters->item(i, j);
            
            if (tableItem->flags() != Qt::NoItemFlags) {
                values << tableItem->text().toDouble();
            }
        }
        
        groupValues.insert(parameter->getGroups().at(j), values);
    }
    
    parameter->setGroupValues(groupValues);
    
    QDialog::accept();
}