#include <ui/coordinate_file_dialog.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QRadioButton>
#include <iostream>

CoordinateFileDialog::CoordinateFileDialog(QWidget *parent, const QString &caption, const QString &directory, const CoordinateFileDialogFilter &filter) :
    QFileDialog(parent, caption, directory, ""),
    rdoUtm(nullptr),
    rdoLatLong(nullptr)
{
    QStringList filters;
    
    if (filter & CoordinateFileDialog::FILTER_TEXT) {
        filters << "Text file (*.txt *xyz)";
    }
    
    if (filter & CoordinateFileDialog::FILTER_KML) {
        filters << "Keyhole Markup Language file (*.kml)";
    }
    
    this->setNameFilters(filters);
}

void CoordinateFileDialog::addCoordinateCheckBoxes() {
    rdoUtm = new QRadioButton("Projected Coordinate System", this);
    rdoUtm->setObjectName("rdoUtm");
    rdoUtm->setChecked(true);
    
    rdoLatLong = new QRadioButton("Geographic Coordinate System (Lat/Long)", this);
    rdoLatLong->setObjectName("rdoLatLong");
    
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(rdoUtm);
    horizontalLayout->addWidget(rdoLatLong);
    
    QGridLayout *gridLayout = this->findChild<QGridLayout*>();
    gridLayout->addLayout(horizontalLayout, gridLayout->rowCount(), 0, 1, 2);
}

bool CoordinateFileDialog::isLatitudeLongitudeChecked() const {
    QRadioButton *rdoLatLong = this->findChild<QRadioButton*>("rdoLatLong");
    return rdoLatLong->isChecked();
}

int CoordinateFileDialog::exec() {
    setOption(QFileDialog::DontUseNativeDialog);
    addCoordinateCheckBoxes();
    
    return QFileDialog::exec();
}

void CoordinateFileDialog::on_fileTypeCombo_currentIndexChanged(const QString &filter) {
    if (rdoUtm && rdoLatLong) {
        rdoUtm->setDisabled(filter.contains("kml"));
        rdoLatLong->setChecked(filter.contains("kml"));
    }
}