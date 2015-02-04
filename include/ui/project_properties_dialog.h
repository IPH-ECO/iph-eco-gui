#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "include/services/project_service.h"

namespace Ui {
class ProjectPropertiesDialog;
}

class ProjectPropertiesDialog : public QDialog {
    Q_OBJECT

public:
    explicit ProjectPropertiesDialog(QWidget *parent = 0);
    ~ProjectPropertiesDialog();
private slots:


    void on_btnBox_clicked(QAbstractButton *button);

private:
    Ui::ProjectPropertiesDialog *ui;

    bool isFormValid();
};

#endif // PROJECTPROPERTIESDIALOG_H
