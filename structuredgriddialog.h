#ifndef STRUCTUREDGRIDDIALOG_H
#define STRUCTUREDGRIDDIALOG_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class StructuredGridDialog;
}

class StructuredGridDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StructuredGridDialog(QWidget *parent = 0);
    ~StructuredGridDialog();

private slots:
    void on_boundaryFileButton_clicked();

private:
    Ui::StructuredGridDialog *ui;
};

#endif // STRUCTUREDGRIDDIALOG_H
