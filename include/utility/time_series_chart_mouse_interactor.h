#ifndef TIME_SERIES_CHART_MOUSE_INTERACTOR_H
#define TIME_SERIES_CHART_MOUSE_INTERACTOR_H

#include "mesh_mouse_interactor.h"

#include <QMap>
#include <vtkActor.h>
#include <vtkIdTypeArray.h>
#include <vtkUnstructuredGrid.h>

class TimeSeriesChartMouseInteractor : public MeshMouseInteractor {
private:
    QMap<QString, vtkSmartPointer<vtkIdTypeArray> > cellIdMap;
    QMap<vtkIdType, vtkSmartPointer<vtkActor> > cellActors;
    
    void renderCellId(const vtkIdType &cellId, vtkSmartPointer<vtkUnstructuredGrid> layerGrid);
public:
    static TimeSeriesChartMouseInteractor* New();
    vtkTypeMacro(TimeSeriesChartMouseInteractor, vtkInteractorStyleRubberBandPick);
    
    TimeSeriesChartMouseInteractor();
    
    void pickCell(vtkSmartPointer<vtkUnstructuredGrid> layerGrid, const QString &layerKey);
    vtkSmartPointer<vtkIdTypeArray> getCellIdArray(const QString &layerKey) const;
    void getCellColor(const vtkIdType &cellId, double color[3]) const;
    void removePickedCells(const QString &layerKey);
    void renderCellsIds(const QString &layerKey, vtkSmartPointer<vtkUnstructuredGrid> grid, vtkSmartPointer<vtkIdTypeArray> cellsIds);
};

#endif // TIME_SERIES_CHART_MOUSE_INTERACTOR_H