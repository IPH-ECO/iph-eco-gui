#ifndef WATER_QUALITY_REPOSITORY_H
#define WATER_QUALITY_REPOSITORY_H

#include <utility/database_utility.h>
#include <domain/water_quality_parameter.h>
#include <domain/water_quality_configuration.h>

class WaterQualityRepository {
private:
    static WaterQualityRepository *instance;
    
    QList<WaterQualityParameter*> parameters;
    QList<WaterQualityParameter*> rootParameters;
    
    WaterQualityRepository();
public:
    static WaterQualityRepository* getInstance();
    ~WaterQualityRepository();

    void buildParameters(WaterQualityConfiguration *configuration);
    WaterQualityParameter* findParameterByName(const QString &name, const WaterQualityParameterSection &section);
    QList<WaterQualityParameter*> getParameters() const;
    QList<WaterQualityParameter*> getRootParameters() const;
};

#endif // WATER_QUALITY_REPOSITORY_H