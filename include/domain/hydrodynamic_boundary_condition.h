#ifndef HYDRODYNAMIC_BOUNDARY_CONDITION_H
#define HYDRODYNAMIC_BOUNDARY_CONDITION_H

#include "boundary_condition.h"

#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <vtkActor2D.h>
#include <vtkActor.h>
#include <QSet>

class HydrodynamicBoundaryCondition : public BoundaryCondition {
    Q_OBJECT
private:
    QSet<vtkIdType> objectIds;
    QString cellColor;
    bool verticalIntegratedOutflow;
    double minimumElevation;
    double maximumElevation;
    
    // Transient attributes
    vtkSmartPointer<vtkActor> selectionActor;
    vtkSmartPointer<vtkActor2D> labelsActor;

public:
    HydrodynamicBoundaryCondition();

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
    bool useVerticalIntegratedOutflow() const;
    void setVerticalIntegratedOutflow(bool verticalIntegratedOutflow);
    double getMinimumElevation() const;
    void setMinimumElevation(const double &minimumElevation);
    double getMaximumElevation() const;
    void setMaximumElevation(const double &maximumElevation);
    vtkSmartPointer<vtkActor> getSelectionActor() const;
    void setSelectionActor(vtkSmartPointer<vtkActor> selectionActor);
    vtkSmartPointer<vtkActor2D> getLabelsActor() const;
    void setLabelsActor(vtkSmartPointer<vtkActor2D> labelsActor);

    SimulationDataType::BoundaryCondition toSimulationDataType(Mesh *mesh) const;

signals:
    void objectIdsChanged();
};

#endif // HYDRODYNAMIC_BOUNDARY_CONDITION_H