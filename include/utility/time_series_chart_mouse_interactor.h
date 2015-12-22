#ifndef TIME_SERIES_CHART_MOUSE_INTERACTOR_H
#define TIME_SERIES_CHART_MOUSE_INTERACTOR_H

#include "mesh_mouse_interactor.h"

#include <QMap>
#include <QColor>
#include <vtkPolyData.h>
#include <vtkIdTypeArray.h>

class TimeSeriesChartMouseInteractor : public MeshMouseInteractor {
private:
	vtkSmartPointer<vtkPolyData> meshPolyData;
	QMap<QString, vtkSmartPointer<vtkIdTypeArray> > cellIdMap;
	QMap<vtkIdType, QColor> cellColorMap;
    QString layerKey;
public:
    static TimeSeriesChartMouseInteractor* New();
    vtkTypeMacro(TimeSeriesChartMouseInteractor, vtkInteractorStyleRubberBandPick);
    
    void setLayerKey(const QString &layerKey);
    void pickCell();
};

#endif // TIME_SERIES_CHART_MOUSE_INTERACTOR_H