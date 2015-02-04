#ifndef UNSTRUCTURED_MESH_WIDGET_H
#define UNSTRUCTURED_MESH_WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QInputDialog>

namespace Ui {
class UnstructuredMeshWidget;
}

class UnstructuredMeshWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UnstructuredMeshWidget(QWidget *parent = 0);
    ~UnstructuredMeshWidget();

private slots:
    void on_newMeshButton_clicked();
    void on_boundaryFileBrowserButton_clicked();
    void on_resetMeshButton_clicked();

private:
    Ui::UnstructuredMeshWidget *ui;
};

#endif // UNSTRUCTURED_MESH_WIDGET_H
