#ifndef SIMULATION_MODEL_H
#define SIMULATION_MODEL_H

namespace SimulationModel {
	extern "C" {
		struct StructuredMesh {
			vtkIdType numberOfElements; // number of squares
			double resolution; // mesh resolution
			double *xCoordinates; // x coordinates
			double *yCoordinates; // y coordinates
			vtkIdType *northNeighbors; // north neighbors
			vtkIdType *westNeighbors; // west neighbors
			vtkIdType *southNeighbors; // south neighbors
			vtkIdType *eastNeighbors; // east neighbors
		};

		struct UnstructuredMesh {
			vtkIdType numberOfPoints; // number of points
			double *xCoordinateIds; // x coordinates indexes
			double *yCoordinateIds; // y coordinates indexes
			vtkIdType numberOfElements; // number of triangles
			vtkIdType **vertexIds; // vertices indexes for each triangle
		};

		struct GridData {
			vtkIdType numberOfElements; // number of cells
			double *bathymetry; // batymetry
			double *roughness; // roughness
            bool hasWindReductionCoefficient;
			double *windReductionCoefficient; // wind reduction coefficient
            bool hasWetland;
			double *wetland; // wetland
            bool has50GrainSize;
			double *d50GrainSize; // sediment median diameter
            bool hasOrganicMatterFraction;
			double *organicMatterFraction; // organic matter fraction
            bool hasQuotaOfImpermeableLayer;
			double *quotaOfImpermeableLayer; // quota of impermeable layer
		};
	}
}

#endif // SIMULATION_MODEL_H