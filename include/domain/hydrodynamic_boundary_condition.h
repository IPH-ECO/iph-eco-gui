#ifndef HYDRODYNAMIC_BOUNDARY_CONDITION_H
#define HYDRODYNAMIC_BOUNDARY_CONDITION_H

#include "boundary_condition.h"
#include "non_vertically_integrated_range.h"

#include <vtkSmartPointer.h>
#include <vtkIdTypeArray.h>
#include <vtkActor2D.h>
#include <vtkActor.h>
#include <QSet>

class HydrodynamicBoundaryCondition : public BoundaryCondition {
    Q_OBJECT
private:
    bool verticallyIntegrated;
    QSet<vtkIdType> objectIds;
    QString cellColor;
    QSet<NonVerticallyIntegratedRange*> nonVerticallyIntegratedRanges;
    
    // Transient attributes
    vtkSmartPointer<vtkActor> selectionActor;
    vtkSmartPointer<vtkActor2D> labelsActor;

public:
    HydrodynamicBoundaryCondition();
    ~HydrodynamicBoundaryCondition();

    bool isVerticallyIntegrated() const;
    void setVerticallyIntegrated(const bool &verticallyIntegrated);
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
    QSet<NonVerticallyIntegratedRange*> getNonVerticallyIntegratedRanges() const;
    void setNonVerticallyIntegratedRanges(const QSet<NonVerticallyIntegratedRange*> &nonVerticallyIntegratedRanges);
    void addNonVerticallyIntegratedRange(NonVerticallyIntegratedRange *nonVerticallyIntegratedRanges);
    void removeNonVerticallyIntegratedRange(NonVerticallyIntegratedRange *nonVerticallyIntegratedRanges);
    vtkSmartPointer<vtkActor> getSelectionActor() const;
    void setSelectionActor(vtkSmartPointer<vtkActor> selectionActor);
    vtkSmartPointer<vtkActor2D> getLabelsActor() const;
    void setLabelsActor(vtkSmartPointer<vtkActor2D> labelsActor);

    SimulationDataType::BoundaryCondition toSimulationDataType(Mesh *mesh) const;

signals:
    void objectIdsChanged();
};

#endif // HYDRODYNAMIC_BOUNDARY_CONDITION_H
