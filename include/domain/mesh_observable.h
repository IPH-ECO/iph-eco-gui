#ifndef MESH_OBSERVABLE_H
#define MESH_OBSERVABLE_H

#include "observable.h"

class MeshObservable : public Observable {
private:
    qint64 observableBoundaryFileSize;
    qint64 observableCurrentBoundaryFileByte;
    bool _cancelCurrentTask;

protected:
    void setObservableBoundaryFileSize(qint64 observableBoundaryFileSize) {
        this->observableBoundaryFileSize = observableBoundaryFileSize;
    }

    void setObservableCurrentBoundaryFileSize(qint64 observableCurrentBoundaryFileByte) {
        this->observableCurrentBoundaryFileByte = observableCurrentBoundaryFileByte;
        this->notify();
    }

public:
    MeshObservable() : _cancelCurrentTask(false) {}

    qint64 getProgress() {
        return (this->observableBoundaryFileSize / (double) this->observableCurrentBoundaryFileByte) * 100;
    }

    bool isCurrentTaskCanceled() {
        return this->_cancelCurrentTask;
    }

    void cancelCurrentTask() {
        this->_cancelCurrentTask = true;
    }
};

#endif // MESH_OBSERVABLE_H
