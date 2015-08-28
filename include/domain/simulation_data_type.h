#ifndef SIMULATION_DATA_TYPE_H
#define SIMULATION_DATA_TYPE_H

#include <vtkIdList.h>

namespace SimulationDataType {
    extern "C" {
        struct StructuredMesh {
            vtkIdType numberOfElements;
			unsigned int resolution;
            double *xCoordinates;
            double *yCoordinates;
            vtkIdType *northNeighbors;
            vtkIdType *westNeighbors;
            vtkIdType *southNeighbors;
            vtkIdType *eastNeighbors;
        };
        
        struct UnstructuredMesh {
            vtkIdType numberOfPoints;
            double *xCoordinates;
            double *yCoordinates;
            vtkIdType numberOfElements;
            vtkIdType *verticeIds;
        };
        
        struct GridData {
            vtkIdType numberOfElements;
            double *weights;
            int type;
        };
        
        struct GridDataConfiguration {
            int numberOfLayers;
            SimulationDataType::GridData *layers;
            bool isStructured;
            SimulationDataType::StructuredMesh structuredMesh;
            SimulationDataType::UnstructuredMesh unstructuredMesh;
        };
        
        struct HydrodynamicParameter {
            char *name;
            int length;
            double value;
        };
        
        struct TimeSeries {
            int timeStampSize;
            char *timeStamp;
            double value;
        };
        
        struct BoundaryCondition {
            int conditionType;
            int numberOfObjects;
            vtkIdType *objectIds;
            int conditionFunction;
            double constantValue;
            int timeSeriesListSize;
            SimulationDataType::TimeSeries *timeSeriesList;
            bool verticalIntegratedOutflow;
            double quota;
        };
        
        struct HydrodynamicConfiguration {
            int numberOfParameters;
            SimulationDataType::HydrodynamicParameter *parameters;
            int numberOfBoundaryConditions;
            SimulationDataType::BoundaryCondition *boundaryConditions;
            SimulationDataType::GridDataConfiguration gridDataConfiguration;
        };

        struct Simulation {
            int labelLength;
            char *label;
            int simulationType;
            unsigned int initialTime;
            double period;
            int stepTime;
            int layersLength;
            double *layers;
            HydrodynamicConfiguration *hydrodynamicConfiguration;
            int observationsLength;
            char *observations;
        };
    }
}

#endif // SIMULATION_DATA_TYPE_H
