#ifndef SAVEONCLOSEDIALOG_H
#define SAVEONCLOSEDIALOG_H

#include <QDialog>

#include "include/domain/project.h"
#include "include/services/project_service.h"

namespace Ui {
class SaveOnCloseDialog;
}

class SaveOnCloseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveOnCloseDialog(QWidget *parent = 0, Project *project = 0);
    ~SaveOnCloseDialog();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SaveOnCloseDialog *ui;
    Project *project;
};

#endif // SAVEONCLOSEDIALOG_H
