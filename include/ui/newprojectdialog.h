#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDir>
#include <QSqlDatabase>
#include <QAbstractButton>
#include <QMessageBox>

#include "include/services/projectservice.h"

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();

private slots:
    void on_bottomButtonBox_clicked(QAbstractButton * button);

private:
    Ui::NewProjectDialog *ui;
    ProjectService *projectService;

    bool isFormValid();
    void clearForm();
    QString formatProjectAnalysis();
};

#endif // NEWPROJECTDIALOG_H
