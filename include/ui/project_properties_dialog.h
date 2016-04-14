#ifndef PROJECTPROPERTIESDIALOG_H
#define PROJECTPROPERTIESDIALOG_H

#include <QDialog>

namespace Ui {
class ProjectPropertiesDialog;
}

class ProjectPropertiesDialog : public QDialog {
    Q_OBJECT
public:
    explicit ProjectPropertiesDialog(QWidget *parent = nullptr);
    ~ProjectPropertiesDialog();
private:
    Ui::ProjectPropertiesDialog *ui;
    
    bool isFormValid();
    virtual void accept();
};

#endif // PROJECTPROPERTIESDIALOG_H
