#ifndef HYDRODYNAMIC_BOUNDARY_CONDITION_H
#define HYDRODYNAMIC_BOUNDARY_CONDITION_H

#include "boundary_condition.h"
#include "vertical_integrated_range.h"

#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <vtkActor2D.h>
#include <vtkActor.h>
#include <QSet>

class HydrodynamicBoundaryCondition : public BoundaryCondition {
    Q_OBJECT
private:
    bool verticalIntegrated;
    QSet<vtkIdType> objectIds;
    QString cellColor;
    QSet<VerticalIntegratedRange*> verticalIntegratedRanges;
    
    // Transient attributes
    vtkSmartPointer<vtkActor> selectionActor;
    vtkSmartPointer<vtkActor2D> labelsActor;

public:
    HydrodynamicBoundaryCondition();
    ~HydrodynamicBoundaryCondition();

    bool isVerticalIntegrated() const;
    void setVerticalIntegrated(const bool &verticalIntegrated);
    QSet<vtkIdType> getObjectIds() const;
    vtkSmartPointer<vtkIdTypeArray> getVTKObjectIds() const;
    QString getVTKObjectsArrayName() const;
    void setObjectIds(const QSet<vtkIdType> &objectIds);
    void setObjectIds(vtkIdTypeArray* objectIds);
    void setObjectIds(const QString &objectIdsStr);
    void addObjectId(const vtkIdType &objectId);
    QString getObjectIdsStr() const;
    void removeObjectId(const vtkIdType &objectId);
    void clearObjectIds();
    QString getCellColor() const;
    void setCellColor(const QString &cellColor);
    QSet<VerticalIntegratedRange*> getVerticalIntegratedRanges() const;
    void setVerticalIntegratedRanges(const QSet<VerticalIntegratedRange*> &verticalIntegratedRanges);
    void addVerticalIntegratedRange(VerticalIntegratedRange *verticalIntegratedRange);
    void removeVerticalIntegratedRange(VerticalIntegratedRange *verticalIntegratedRange);
    vtkSmartPointer<vtkActor> getSelectionActor() const;
    void setSelectionActor(vtkSmartPointer<vtkActor> selectionActor);
    vtkSmartPointer<vtkActor2D> getLabelsActor() const;
    void setLabelsActor(vtkSmartPointer<vtkActor2D> labelsActor);

    SimulationDataType::BoundaryCondition toSimulationDataType(Mesh *mesh) const;

signals:
    void objectIdsChanged();
};

#endif // HYDRODYNAMIC_BOUNDARY_CONDITION_H
