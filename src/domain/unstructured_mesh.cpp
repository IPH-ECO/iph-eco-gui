#include "include/domain/unstructured_mesh.h"

#include <QFile>
#include <QStringList>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QtMath>

#include <GeographicLib/GeoCoords.hpp>

UnstructuredMesh::UnstructuredMesh() {}

UnstructuredMesh::UnstructuredMesh(QString &_name) : Mesh(_name) {}

void UnstructuredMesh::generate() {
    const MeshPolygon *boundaryPolygon = getBoundaryPolygon();

    if (boundaryPolygon == NULL) {
        throw MeshException("No boundary vertices found.");
    }

    if (cdt.number_of_vertices() > 0) {
        return; // Mesh already generated
    }

    for (int i = 0; i < domain.size(); i++) {
        MeshPolygon meshPolygon = domain.at(i);
        ulong polygonVerticesCount = meshPolygon.size();

        cdt.insert(meshPolygon.vertices_begin(), meshPolygon.vertices_end());

        for (ulong j = 0; j < polygonVerticesCount; j++) {
            Point p1 = meshPolygon[j];
            Point p2 = meshPolygon[j == polygonVerticesCount - 1 ? 0 : j + 1];

            cdt.insert_constraint(p1, p2);
        }
    }

    Criteria criteria(boundaryPolygon->getMinimumAngle(), boundaryPolygon->getMaximumEdgeLength());
    Mesher mesher(cdt, criteria);

    mesher.refine_mesh();

    mark_domains();
}

const CDT* UnstructuredMesh::getCDT() {
    return &cdt;
}

void UnstructuredMesh::buildDomain(const QString &filename) {
    cdt.clear();
    Mesh::buildDomain(filename);
}

void UnstructuredMesh::clear() {
    cdt.clear();
    Mesh::clear();
}

void UnstructuredMesh::mark_domains(CDT::Face_handle start, int index, QList<CDT::Edge>& border) {
    if (start->info().nesting_level != -1) {
        return;
    }

    QList<CDT::Face_handle> queue;

    queue.push_back(start);

    while (!queue.empty()) {
        CDT::Face_handle fh = queue.front();

        queue.pop_front();

        if (fh->info().nesting_level == -1) {
            fh->info().nesting_level = index;

            for (int i = 0; i < 3; i++) {
                CDT::Edge e(fh, i);
                CDT::Face_handle n = fh->neighbor(i);

                if (n->info().nesting_level == -1) {
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

void UnstructuredMesh::mark_domains() {
    for (CDT::All_faces_iterator it = cdt.all_faces_begin(); it != cdt.all_faces_end(); it++) {
        it->info().nesting_level = -1;
    }

    QList<CDT::Edge> border;

    mark_domains(cdt.infinite_face(), 0, border);

    while (!border.empty()) {
        CDT::Edge e = border.front();
        CDT::Face_handle n = e.first->neighbor(e.second);

        border.pop_front();

        if (n->info().nesting_level == -1) {
            mark_domains(n, e.first->info().nesting_level + 1, border);
        }
    }
}
