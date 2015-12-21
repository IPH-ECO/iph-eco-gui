#ifndef HYDRODYNAMIC_DATA_REPOSITORY_H
#define HYDRODYNAMIC_DATA_REPOSITORY_H

#include <QJsonArray>

#include <utility/database_utility.h>
#include <domain/hydrodynamic_configuration.h>

class HydrodynamicDataRepository {
private:
    static HydrodynamicDataRepository *instance;

    QJsonArray jsonParameters;
    QJsonArray jsonProcesses;
    QList<HydrodynamicProcess*> processes;

    HydrodynamicDataRepository();
public:
    static HydrodynamicDataRepository* getInstance();
    ~HydrodynamicDataRepository();

    void buildParameters(HydrodynamicConfiguration *configuration);
    QList<HydrodynamicProcess*> getProcesses(HydrodynamicConfiguration *configuration);
    HydrodynamicProcess* findProcessByName(const QString &name) const;
};

#endif // HYDRODYNAMIC_DATA_REPOSITORY_H