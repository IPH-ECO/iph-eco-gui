#ifndef MESHOBSERVER_H
#define MESHOBSERVER_H

#include <QProgressDialog>

#include "observer.h"
#include "mesh_manager.h"

class MeshObserver : public Observer {
private:
//    MeshObservableEvent event;
    QProgressDialog *progressDialog;

public:
    enum MeshObservableEvent { BOUNDARY_FILE_CONVERSION, MESH_GENERATION };

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
