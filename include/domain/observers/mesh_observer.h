#ifndef MESHOBSERVER_H
#define MESHOBSERVER_H

#include <QProgressDialog>

#include "include/domain/observers/observer.h"
#include "include/domain/managers/mesh_manager.h"

class MeshObserver : public Observer {
private:
    QProgressDialog *progressDialog;

public:
    MeshObserver(Observable *observable) : Observer(observable), progressDialog(NULL) {}

    virtual void update() {
        MeshManager *meshManager = (MeshManager*) this->observable;

        if (this->progressDialog->wasCanceled()) {
            meshManager->cancelCurrentTask();
        } else {
            this->progressDialog->setValue(meshManager->getProgress());
        }
    }

    void setProgressDialog(QProgressDialog *progressDialog) {
        this->progressDialog = progressDialog;
    }
};

#endif // MESHOBSERVER_H
