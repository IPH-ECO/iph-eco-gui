#ifndef HYDRODYNAMIC_BOUNDARY_CONDITION_H
#define HYDRODYNAMIC_BOUNDARY_CONDITION_H

#include "boundary_condition.h"
#include "water_quality_boundary_condition.h"

#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <vtkActor2D.h>
#include <vtkActor.h>
#include <QSet>

class WaterQualityBoundaryCondition;

class HydrodynamicBoundaryCondition : public BoundaryCondition {
    Q_OBJECT
private:
    QSet<vtkIdType> objectIds;
    QString cellColor;
    
    // Transient attributes
    vtkSmartPointer<vtkActor> selectionActor;
    vtkSmartPointer<vtkActor2D> labelsActor;
    QSet<WaterQualityBoundaryCondition*> waterQualityBoundaryConditions;

public:
    HydrodynamicBoundaryCondition();
    ~HydrodynamicBoundaryCondition();

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
    vtkSmartPointer<vtkActor> getSelectionActor() const;
    void setSelectionActor(vtkSmartPointer<vtkActor> selectionActor);
    vtkSmartPointer<vtkActor2D> getLabelsActor() const;
    void setLabelsActor(vtkSmartPointer<vtkActor2D> labelsActor);
    void addWaterQualityBoundaryCondition(WaterQualityBoundaryCondition *waterQualityBoundaryCondition);
    QSet<WaterQualityBoundaryCondition*> getWaterQualityBoundaryConditions() const;

    SimulationDataType::BoundaryCondition toSimulationDataType(Mesh *mesh) const;

signals:
    void objectIdsChanged();
};

#endif // HYDRODYNAMIC_BOUNDARY_CONDITION_H
