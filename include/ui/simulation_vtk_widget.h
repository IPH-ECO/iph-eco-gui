#ifndef SIMULATION_VTK_WIDGET_H
#define SIMULATION_VTK_WIDGET_H

#include "include/domain/simulation.h"
#include "include/ui/mesh_vtk_widget.h"
#include "include/domain/meteorological_station.h"

class SimulationVTKWidget : public MeshVTKWidget {
	Q_OBJECT

public:
	explicit SimulationVTKWidget(QWidget *parent);
	void render(Simulation *simulation, const QString &layer, const QString &component, int frame);
};

#endif // SIMULATION_VTK_WIDGET_H