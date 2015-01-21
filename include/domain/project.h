#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>

namespace IPH {

    namespace Domain {

        class Project {
        private:
            std::string name;
            int version;
            std::string description;
            std::string analysis;
            long selectedGridDataId;
            long selectedHydrodynamicDataId;
            long selectedWaterQualityDataId;
            long selectedSedimentDataId;
            long selectedMeteorologicalDataId;

        public:
            Project();

            void setName(const std::string &name);
            std::string getName() const;

            void setVersion(const int &version);
            int getVersion() const;

            void setDescription(const std::string &description);
            std::string getDescription() const;

            void setAnalysis(const std::string &analysis);
            std::string getAnalysis() const;

            void setSelectedGridDataId(const int &selectedGridDataId);
            long getSelectedGridDataId() const;

            void setSelectedHydrodynamicDataId(const long &selectedHydrodynamicDataId);
            long getSelectedHydrodynamicDataId() const;

            void setSelectedWaterQualityDataId(const long &selectedWaterQualityDataId);
            long getSelectedWaterQualityDataId() const;

            void setSelectedSedimentDataId(const long &selectedSedimentDataId);
            long getSelectedSedimentDataId() const;

            void setSelectedMeteorologicalDataId(const long &selectedMeteorologicalDataId);
            long getSelectedMeteorologicalDataId() const;
        };

    }

}

#endif // PROJECT_H
