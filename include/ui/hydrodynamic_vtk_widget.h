#ifndef HYDRODYNAMIC_VTK_WIDGET_H
#define HYDRODYNAMIC_VTK_WIDGET_H

#include "include/domain/mesh.h"

#include <QVTKWidget>

class HydrodynamicVTKWidget : public QVTKWidget {
private:
    Mesh *currentMesh;

public:
    explicit HydrodynamicVTKWidget(QWidget *parent);
	void render(Mesh *mesh);
};

#endif // HYDRODYNAMIC_VTK_WIDGET_H