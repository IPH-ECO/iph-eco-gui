#ifndef COORDINATE_FILE_DIALOG_H
#define COORDINATE_FILE_DIALOG_H

#include <QWidget>
#include <QFileDialog>
#include <QGridLayout>
#include <QRadioButton>
#include <QHBoxLayout>

class CoordinateFileDialog : public QFileDialog {
    Q_OBJECT
private:
    void addCoordinateCheckBoxes() {
        QRadioButton *rdoLatLong = new QRadioButton("Geographic Coordinate System (Lat/Long)", this);
        rdoLatLong->setObjectName("rdoLatLong");
        rdoLatLong->setChecked(true);
        
        QRadioButton *rdoUtm = new QRadioButton("Projected Coordinate System", this);
        rdoUtm->setObjectName("rdoUtm");
        
        QHBoxLayout *horizontalLayout = new QHBoxLayout();
        horizontalLayout->addWidget(rdoLatLong);
        horizontalLayout->addWidget(rdoUtm);
        
        QGridLayout *gridLayout = this->findChild<QGridLayout*>();
        gridLayout->addLayout(horizontalLayout, gridLayout->rowCount(), 0, 1, 2);
    }
    
public:
    CoordinateFileDialog(QWidget *parent, const QString &caption, const QString &directory, const QString &filter) :
        QFileDialog(parent, caption, directory, filter) {}
    
    bool isLatitudeLongitudeChecked() const {
        QRadioButton *rdoLatLong = this->findChild<QRadioButton*>("rdoLatLong");
        return rdoLatLong->isChecked();
    }
    
    virtual int exec() {
        setOption(QFileDialog::DontUseNativeDialog);
        addCoordinateCheckBoxes();
        
        return QFileDialog::exec();
    }
};

#endif // COORDINATE_FILE_DIALOG_H
