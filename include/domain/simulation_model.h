#ifndef SIMULATION_MODEL_H
#define SIMULATION_MODEL_H

namespace SimulationModel {
	extern "C" {
		struct StructuredMesh {
			vtkIdType nElem; // number of squares
			double dx; // mesh resolution
			double *CoordX; // x coordinates
			double *CoordY; // y coordinates
			vtkIdType *VIZN; // north neighbors
			vtkIdType *VIZO; // west neighbors
			vtkIdType *VIZS; // south neighbors
			vtkIdType *VIZL; // east neighbors
		};

		struct UnstructuredMesh {
			vtkIdType nPoint; // number of points
			double *CoordX_point; // x coordinates indexes
			double *CoordY_point; // y coordinates indexes
			vtkIdType nElem; // number of triangles
			vtkIdType **Connect; // vertices indexes for each triangle
		};

		struct GridData {
			vtkIdType nElem; // number of cells
			double *zBatim; // batymetry
			double *Rug; // roughness
			double *Cvento; // wind reduction coefficient
			double *Wetland; // wetland
			double *D50; // sediment median diameter
			double *forg; // organic material fraction
			double *zSub; // impermeable layer quota
		};
	}
}

#endif // SIMULATION_MODEL_H