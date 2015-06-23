#ifndef PROGRESS_OBSERVER_H
#define PROGRESS_OBSERVER_H

#include <vtkProgressObserver.h>

class ProgressObserver : public QObject, public vtkProgressObserver {
    Q_OBJECT
public:
    static ProgressObserver* New();
    vtkTypeMacro(ProgressObserver, vtkProgressObserver);
    
    virtual void UpdateProgress(double amount) {
        emit updateProgressSignal(amount * 100);
        vtkProgressObserver::UpdateProgress(amount);
    }
signals:
    void updateProgressSignal(int value);
};

#endif // PROGRESS_OBSERVER_H
