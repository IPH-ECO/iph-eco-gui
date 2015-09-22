#ifndef METEOROLOGICAL_VTK_WIDGET_H
#define METEOROLOGICAL_VTK_WIDGET_H

#include "include/ui/mesh_vtk_widget.h"
#include "include/domain/meteorological_station.h"

class MeteorologicalVTKWidget : public MeshVTKWidget {
	Q_OBJECT

public:
	explicit MeteorologicalVTKWidget(QWidget *parent);
    void addStation(MeteorologicalStation *station);
    void removeStation(MeteorologicalStation *station);
    void highlightStation(MeteorologicalStation *station);
	void showStations();
};

#endif // METEOROLOGICAL_VTK_WIDGET_H