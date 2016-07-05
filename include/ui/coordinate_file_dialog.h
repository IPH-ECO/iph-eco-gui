#ifndef COORDINATE_FILE_DIALOG_H
#define COORDINATE_FILE_DIALOG_H

#include <QWidget>
#include <QFileDialog>
#include <QRadioButton>

class CoordinateFileDialog : public QFileDialog {
    Q_OBJECT
private:
    QRadioButton *rdoUtm;
    QRadioButton *rdoLatLong;
    
    void addCoordinateCheckBoxes();
    
public:
    enum CoordinateFileDialogFilter {
        FILTER_TEXT = (1 << 0),
        FILTER_KML = (1 << 1)
    };
    
    CoordinateFileDialog(QWidget *parent, const QString &caption, const QString &directory, const CoordinateFileDialogFilter &filter);
    
    bool isLatitudeLongitudeChecked() const;
    
    virtual int exec();

private slots:
    void on_fileTypeCombo_currentIndexChanged(const QString &type);
};

#endif // COORDINATE_FILE_DIALOG_H
