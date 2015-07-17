#ifndef HYDRODYNAMIC_DATA_REPOSITORY_H
#define HYDRODYNAMIC_DATA_REPOSITORY_H

#include "include/application/iph_application.h"
#include "include/domain/hydrodynamic_parameter.h"
#include "include/domain/hydrodynamic_process.h"

class HydrodynamicDataRepository {
private:
    QList<HydrodynamicParameter*> parameters;
    QList<HydrodynamicProcess*> processes;
public:
    ~HydrodynamicDataRepository();
    QList<HydrodynamicParameter*> getParameters();
    QList<HydrodynamicProcess*> getProcesses();
    
    template<typename T> T* findByName(const QList<T*> &list, const QString &name) const;
    HydrodynamicProcess* findProcessByName(const QString &name) const;
    HydrodynamicParameter* findParameterByName(const QString &name) const;
};

#endif // HYDRODYNAMIC_DATA_REPOSITORY_H