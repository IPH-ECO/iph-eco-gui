#ifndef CELL_UPDATE_DIALOG_H
#define CELL_UPDATE_DIALOG_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class CellUpdateDialog;
}

class CellUpdateDialog : public QDialog {
    Q_OBJECT
private:
    Ui::CellUpdateDialog *ui;
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

public:
    explicit CellUpdateDialog(QWidget *parent = 0);
    ~CellUpdateDialog();

    void setValues(QString gridInformationType, double weight);
    double getWeight();
};

#endif // CELL_UPDATE_DIALOG_H
