#ifndef UNSTRUCTURED_MESH_WIDGET_H
#define UNSTRUCTURED_MESH_WIDGET_H

#include <QWidget>

namespace Ui {
class UnstructuredMeshWidget;
}

class UnstructuredMeshWidget : public QWidget
{
    Q_OBJECT

public:
    explicit UnstructuredMeshWidget(QWidget *parent = 0);
    ~UnstructuredMeshWidget();

private:
    Ui::UnstructuredMeshWidget *ui;
};

#endif // UNSTRUCTURED_MESH_WIDGET_H
