#ifndef STRUCTURED_MESH_VTK_WIDGET_H
#define STRUCTURED_MESH_VTK_WIDGET_H

#include <QWidget>
#include <QVTKWidget.h>
#include <vtkAutoInit.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

#include "include/domain/structured_mesh.h"

class StructuredMeshVTKWidget : public QVTKWidget {
	Q_OBJECT
public:
    StructuredMeshVTKWidget(QWidget *parent);
    ~StructuredMeshVTKWidget();
    void setMesh(StructuredMesh *mesh);
};

#endif // STRUCTURED_MESH_VTK_WIDGET_H
