#ifndef UNSTRUCTUREDGRIDDIALOG_H
#define UNSTRUCTUREDGRIDDIALOG_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class UnstructuredGridDialog;
}

namespace IPH {

    namespace UI {

        class UnstructuredGridDialog : public QDialog
        {
            Q_OBJECT

        public:
            explicit UnstructuredGridDialog(QWidget *parent = 0);
            ~UnstructuredGridDialog();

        private slots:
            void on_boundaryFileButton_clicked();

        private:
            Ui::UnstructuredGridDialog *ui;
        };

    }

}

#endif // UNSTRUCTUREDGRIDDIALOG_H
