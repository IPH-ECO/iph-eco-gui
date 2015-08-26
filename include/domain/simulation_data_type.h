#ifndef SIMULATION_DATA_TYPE_H
#define SIMULATION_DATA_TYPE_H

#include <vtkIdList.h>

namespace SimulationDataType {
    extern "C" {
        struct StructuredMesh {
            vtkIdType numberOfElements;
            uint resolution;
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
            int type;
            int numberOfObjects;
            vtkIdType *objectIds;
            int function;
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
    }
}

#endif // SIMULATION_DATA_TYPE_H
