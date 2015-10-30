#ifndef SIMULATION_VTK_WIDGET_H
#define SIMULATION_VTK_WIDGET_H

#include "include/ui/mesh_vtk_widget.h"
#include "include/domain/meteorological_station.h"

class SimulationVTKWidget : public MeshVTKWidget {
	Q_OBJECT

public:
	explicit SimulationVTKWidget(QWidget *parent);
	void render(const QString &meshVTKFile);
};

#endif // SIMULATION_VTK_WIDGET_H