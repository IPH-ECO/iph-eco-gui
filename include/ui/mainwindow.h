#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include "newprojectdialog.h"
#include "projectpropertiesdialog.h"
#include "structuredgriddialog.h"
#include "unstructuredgriddialog.h"

namespace Ui {
class MainWindow;
}

namespace IPH {

    namespace UI {

        class MainWindow : public QMainWindow
        {
            Q_OBJECT

        public:
            explicit MainWindow(QWidget *parent = 0);
            ~MainWindow();

        private slots:
            void on_actionOpenProject_triggered();
            void on_actionSaveAsProject_triggered();

        private:
            Ui::MainWindow *ui;
        };

    }
}

#endif // MAINWINDOW_H
