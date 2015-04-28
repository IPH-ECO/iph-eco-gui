#ifndef GRID_DATA_UPDATE_DIALOG_H
#define GRID_DATA_UPDATE_DIALOG_H

#include <QDialog>

namespace Ui {
class GridDataUpdateDialog;
}

class GridDataUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridDataUpdateDialog(QWidget *parent = 0);
    ~GridDataUpdateDialog();

private:
    Ui::GridDataUpdateDialog *ui;
};

#endif // GRID_DATA_UPDATE_DIALOG_H
