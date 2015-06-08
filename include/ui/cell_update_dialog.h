#ifndef CELL_UPDATE_DIALOG_H
#define CELL_UPDATE_DIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QSet>
#include <QList>

namespace Ui {
class CellUpdateDialog;
}

class CellUpdateDialog : public QDialog {
    Q_OBJECT
private:
    Ui::CellUpdateDialog *ui;
    // QSet<CellInfo*> cellInfoSet;
private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

public:
    explicit CellUpdateDialog(QWidget *parent);
    ~CellUpdateDialog();

    // QList<CellInfo> getCellInfoList();
};

#endif // CELL_UPDATE_DIALOG_H
