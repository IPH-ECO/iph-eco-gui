#ifndef NEWPROJECTDIALOG_H
#define NEWPROJECTDIALOG_H

#include <QDialog>

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog {
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();

private:
    Ui::NewProjectDialog *ui;

    virtual void accept();
    bool isFormValid();
    
signals:
    void enableParentMenu(bool enable);
};

#endif // NEWPROJECTDIALOG_H
