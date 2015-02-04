#include "include/ui/unstructured_mesh_widget.h"
#include "ui_unstructured_mesh_widget.h"

UnstructuredMeshWidget::UnstructuredMeshWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UnstructuredMeshWidget)
{
    ui->setupUi(this);
}

UnstructuredMeshWidget::~UnstructuredMeshWidget()
{
    delete ui;
}
