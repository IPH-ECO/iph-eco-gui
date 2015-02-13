#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QDebug>
#include <QDir>
#include <QSqlDatabase>
#include <QAbstractButton>
#include <QMessageBox>
#include <QDialogButtonBox>

#include "include/services/project_service.h"

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();

private slots:
    void on_btnBox_clicked(QAbstractButton *button);

signals:
    void enableParentMenu(bool enable);

private:
    Ui::NewProjectDialog *ui;

    bool isFormValid();
};

#endif // NEWPROJECTDIALOG_H
