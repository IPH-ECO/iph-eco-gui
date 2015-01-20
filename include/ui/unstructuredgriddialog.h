#ifndef UNSTRUCTUREDGRIDDIALOG_H
#define UNSTRUCTUREDGRIDDIALOG_H

#include <QDialog>

namespace Ui {
class UnstructuredGridDialog;
}

class UnstructuredGridDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UnstructuredGridDialog(QWidget *parent = 0);
    ~UnstructuredGridDialog();

private:
    Ui::UnstructuredGridDialog *ui;
};

#endif // UNSTRUCTUREDGRIDDIALOG_H
