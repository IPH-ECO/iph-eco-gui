#ifndef MESH_OBSERVABLE_H
#define MESH_OBSERVABLE_H

#include "observable.h"

class MeshObservable : public Observable {
private:
    qint64 observableBoundaryFileSize;
    qint64 observableCurrentBoundaryFileByte;
    bool currentTaskCanceled;

protected:
    void setObservableBoundaryFileSize(qint64 observableBoundaryFileSize) {
        this->observableBoundaryFileSize = observableBoundaryFileSize;
    }

    void setObservableCurrentBoundaryFileSize(qint64 observableCurrentBoundaryFileByte) {
        this->observableCurrentBoundaryFileByte = observableCurrentBoundaryFileByte;
        this->notify();
    }

public:
    MeshObservable() : currentTaskCanceled(false) {}

    qint64 getProgress() {
        return (this->observableBoundaryFileSize / (double) this->observableCurrentBoundaryFileByte) * 100;
    }

    bool isCurrentTaskCanceled() {
        return this->currentTaskCanceled;
    }

    void cancelCurrentTask() {
        this->currentTaskCanceled = true;
    }
};

#endif // MESH_OBSERVABLE_H
