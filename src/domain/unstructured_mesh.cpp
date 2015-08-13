#include "include/domain/unstructured_mesh.h"

#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>
#include <QMap>

UnstructuredMesh::UnstructuredMesh() {}

void UnstructuredMesh::generate() {
    if (boundaryPolygon == nullptr) {
        throw MeshException("No boundary defined.");
    }

    QList<MeshPolygon*> polygons = islands;
    CDT cdt;
    
    polygons.prepend(boundaryPolygon);
    
    for (QList<MeshPolygon*>::const_iterator it = polygons.begin(); it != polygons.end(); it++) {
        vtkPolygon *polygon = (*it)->getFilteredPolygon();
        vtkIdType numberOfPoints = polygon->GetPoints()->GetNumberOfPoints();

        for (vtkIdType i = 0; i < numberOfPoints; i++) {
            double vtkPoint1[3], vtkPoint2[3];
            vtkIdType j = i == numberOfPoints - 1 ? 0 : i + 1;

            polygon->GetPoints()->GetPoint(i, vtkPoint1);
            polygon->GetPoints()->GetPoint(j, vtkPoint2);

            Point cgalPoint1(vtkPoint1[0], vtkPoint1[1]);
            Point cgalPoint2(vtkPoint2[0], vtkPoint2[1]);

            cdt.insert(cgalPoint1);
            cdt.insert_constraint(cgalPoint1, cgalPoint2);
        }
    }

    Criteria criteria(boundaryPolygon->getMinimumAngleInCGALRepresentation(), boundaryPolygon->getMaximumEdgeLength());
    Mesher mesher(cdt, criteria);

    mesher.refine_mesh();

    /*for (QList<MeshPolygon>::const_iterator it = domain.begin(); it != domain.end(); it++) {
        if (!it->isRefinementArea()) {
            continue;
        }

        cdt.insert(it->vertices_begin(), it->vertices_end());

        Criteria criteria(it->getMinimumAngleInCGALRepresentation(), it->getMaximumEdgeLength());

        mesher.set_criteria(criteria);
        mesher.refine_mesh();
    }*/

    mark_domains(cdt);

    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();
    meshPolyData = vtkSmartPointer<vtkPolyData>::New();
    QMap<Point, vtkIdType> pointsMap;
    vtkIdType i = 0;
    

    for (CDT::Finite_faces_iterator fit = cdt.finite_faces_begin(); fit != cdt.finite_faces_end(); ++fit) {
        if (!fit->info().isInDomain()) {
            continue;
        }

        CGAL::Triangle_2<K> cgalTriangle = cdt.triangle(fit);
        Point a(cgalTriangle[0]), b(cgalTriangle[1]), c(cgalTriangle[2]);
		vtkSmartPointer<vtkTriangle> _vtkTriangle = vtkSmartPointer<vtkTriangle>::New();
        
        if (!pointsMap.contains(a)) {
            pointsMap.insert(a, i++);
            points->InsertNextPoint(a.x(), a.y(), 0.0);
        }
        if (!pointsMap.contains(b)) {
            pointsMap.insert(b, i++);
            points->InsertNextPoint(b.x(), b.y(), 0.0);
        }
        if (!pointsMap.contains(c)) {
            pointsMap.insert(c, i++);
            points->InsertNextPoint(c.x(), c.y(), 0.0);
        }

		_vtkTriangle->GetPointIds()->SetNumberOfIds(3);
        _vtkTriangle->GetPointIds()->SetId(0, pointsMap.value(a));
        _vtkTriangle->GetPointIds()->SetId(1, pointsMap.value(b));
        _vtkTriangle->GetPointIds()->SetId(2, pointsMap.value(c));

        triangles->InsertNextCell(_vtkTriangle);
    }

    meshPolyData->SetPoints(points);
    meshPolyData->SetPolys(triangles);
    
    generateBoundaryPolyData();
}

bool UnstructuredMesh::instanceOf(const QString &type) {
    return type.contains("UnstructuredMesh");
}

void UnstructuredMesh::mark_domains(CDT &cdt, CDT::Face_handle start, int index, QList<CDT::Edge>& border) {
    if (start->info().getNestingLevel() != -1) {
        return;
    }

    QList<CDT::Face_handle> queue;

    queue.push_back(start);

    while (!queue.empty()) {
        CDT::Face_handle fh = queue.front();

        queue.pop_front();

        if (fh->info().getNestingLevel() == -1) {
            fh->info().setNestingLevel(index);

            for (int i = 0; i < 3; i++) {
                CDT::Edge e(fh, i);
                CDT::Face_handle n = fh->neighbor(i);

                if (n->info().getNestingLevel() == -1) {
                    if (cdt.is_constrained(e)) {
                        border.push_back(e);
                    } else {
                        queue.push_back(n);
                    }
                }
            }
        }
    }
}

void UnstructuredMesh::mark_domains(CDT &cdt) {
    for (CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); it++) {
        it->info().setNestingLevel(-1);
    }

    QList<CDT::Edge> border;

    mark_domains(cdt, cdt.infinite_face(), 0, border);

    while (!border.empty()) {
        CDT::Edge e = border.front();
        CDT::Face_handle n = e.first->neighbor(e.second);

        border.pop_front();

        if (n->info().getNestingLevel() == -1) {
            mark_domains(cdt, n, e.first->info().getNestingLevel() + 1, border);
        }
    }
}