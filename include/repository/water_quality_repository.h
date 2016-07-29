#ifndef WATER_QUALITY_REPOSITORY_H
#define WATER_QUALITY_REPOSITORY_H

#include <utility/database_utility.h>
#include <domain/water_quality_parameter.h>
#include <domain/water_quality_configuration.h>

class WaterQualityRepository {
private:
    static WaterQualityRepository *instance;
    
    QList<WaterQualityParameter*> parameters;
    QList<WaterQualityParameter*> structure;
    QList<WaterQualityParameter*> rootParameters;
    QList<WaterQualityParameter*> rootStructure;
    
    WaterQualityRepository();
public:
    static WaterQualityRepository* getInstance();
    ~WaterQualityRepository();

    void buildParameters(WaterQualityConfiguration *configuration);
    WaterQualityParameter* findParameterByName(const QString &name, const WaterQualityParameterSection &section);
    WaterQualityParameter* findParameterByDiagramItem(const QString &diagramItem);
    QList<WaterQualityParameter*> getParameters(const WaterQualityParameterSection &section) const;
    QList<WaterQualityParameter*> getRootParameters(const WaterQualityParameterSection &section) const;
};

#endif // WATER_QUALITY_REPOSITORY_H