#ifndef GRID_DATA_DIALOG_H
#define GRID_DATA_DIALOG_H

#include <QDialog>

namespace Ui {
class GridDataDialog;
}

class GridDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridDataDialog(QWidget *parent = 0);
    ~GridDataDialog();

private:
    Ui::GridDataDialog *ui;
};

#endif // GRID_DATA_DIALOG_H
