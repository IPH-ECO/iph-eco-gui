#ifndef STRUCTUREDGRIDDIALOG_H
#define STRUCTUREDGRIDDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QAbstractButton>
#include <QMessageBox>

#include "include/services/gridservice.h"
#include "include/application/iphapplication.h"

namespace Ui {
class StructuredGridDialog;
}

class StructuredGridDialog : public QDialog {
    Q_OBJECT

public:
    explicit StructuredGridDialog(QWidget *parent = 0);
    ~StructuredGridDialog();

private slots:
    void on_boundaryFileButton_clicked();
    void on_bottomButtonBox_clicked(QAbstractButton *button);

private:
    Ui::StructuredGridDialog *ui;

    bool isValidForm();
    void clearForm();
};

#endif // STRUCTUREDGRIDDIALOG_H
