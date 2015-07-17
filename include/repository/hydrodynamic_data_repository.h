#ifndef HYDRODYNAMIC_DATA_REPOSITORY_H
#define HYDRODYNAMIC_DATA_REPOSITORY_H

#include "include/utility/database_utility.h"
#include "include/domain/hydrodynamic_parameter.h"
#include "include/domain/hydrodynamic_process.h"

class HydrodynamicDataRepository {
private:
    DatabaseUtility *databaseUtility;
    QList<HydrodynamicParameter*> parameters;
    QList<HydrodynamicProcess*> processes;
    
    template<typename T> T* findByName(const QList<T*> &list, const QString &name) const;
public:
    HydrodynamicDataRepository();
    ~HydrodynamicDataRepository();
    QList<HydrodynamicParameter*> getParameters();
    QList<HydrodynamicProcess*> getProcesses();
    
    HydrodynamicProcess* findProcessByName(const QString &name) const;
    HydrodynamicParameter* findParameterByName(const QString &name) const;
};

#endif // HYDRODYNAMIC_DATA_REPOSITORY_H