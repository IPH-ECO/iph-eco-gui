#-------------------------------------------------
#
# Project created by QtCreator 2015-01-16T00:31:49
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iph-eco
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/domain/project.cpp \
    src/application/iph_application.cpp \
    src/dao/project_dao.cpp \
    src/exceptions/database_exception.cpp \
    src/services/project_service.cpp \
    src/ui/main_window.cpp \
    src/ui/new_project_dialog.cpp \
    src/ui/project_properties_dialog.cpp \
    src/utility/database_utility.cpp \
    src/exceptions/mesh_exception.cpp \
    src/domain/unstructured_mesh.cpp \
    src/domain/structured_mesh.cpp \
    src/domain/mesh.cpp \
    src/ui/unstructured_mesh_dialog.cpp \
    src/ui/unstructured_mesh_opengl_widget.cpp \
    src/ui/structured_mesh_opengl_widget.cpp \
    src/domain/mesh_polygon.cpp \
    src/ui/structured_mesh_dialog.cpp \
    src/ui/grid_data_dialog.cpp \
    src/domain/grid_data.cpp \
    src/ui/grid_data_opengl_widget.cpp \
    src/domain/grid_data_configuration.cpp \
    src/exceptions/grid_data_exception.cpp \
    src/ui/grid_information_dialog.cpp \
    src/ui/cell_update_dialog.cpp

HEADERS  += \
    include/domain/project.h \
    include/application/iph_application.h \
    include/dao/project_dao.h \
    include/exceptions/database_exception.h \
    include/services/project_service.h \
    include/ui/main_window.h \
    include/ui/new_project_dialog.h \
    include/ui/project_properties_dialog.h \
    include/utility/database_utility.h \
    include/exceptions/mesh_exception.h \
    include/domain/unstructured_mesh.h \
    include/domain/structured_mesh.h \
    include/domain/mesh.h \
    include/ui/unstructured_mesh_dialog.h \
    include/ui/unstructured_mesh_opengl_widget.h \
    include/ui/structured_mesh_opengl_widget.h \
    include/utility/delaunay_triangulation_definitions.h \
    include/domain/mesh_polygon.h \
    include/ui/structured_mesh_dialog.h \
    include/ui/grid_data_dialog.h \
    include/domain/grid_data.h \
    include/ui/grid_data_opengl_widget.h \
    include/domain/grid_data_configuration.h \
    include/exceptions/grid_data_exception.h \
    include/domain/grid_data_point.h \
    include/ui/grid_information_dialog.h \
    include/domain/grid_data_polygon.h \
    include/domain/quad.h \
    include/domain/grid_information_type.h \
    include/ui/cell_update_dialog.h \
    include/domain/cell_info.h

FORMS    += \
    include/ui/main_window.ui \
    include/ui/new_project_dialog.ui \
    include/ui/project_properties_dialog.ui \
    include/ui/unstructured_mesh_dialog.ui \
    include/ui/structured_mesh_dialog.ui \
    include/ui/grid_data_dialog.ui \
    include/ui/grid_information_dialog.ui \
    include/ui/cell_update_dialog.ui

macx: QMAKE_CXXFLAGS += -Wno-redeclared-class-member -Wno-unused-parameter

macx: LIBS += -L/usr/local/Cellar/geographiclib/1.40/lib/ -lGeographic
macx: INCLUDEPATH += /usr/local/Cellar/geographiclib/1.40/include
macx: DEPENDPATH += /usr/local/Cellar/geographiclib/1.40/include

macx: LIBS += -L/usr/local/Cellar/boost/1.57.0/lib/ -lboost_system
macx: INCLUDEPATH += /usr/local/Cellar/boost/1.57.0/include
macx: DEPENDPATH += /usr/local/Cellar/boost/1.57.0/include

macx: LIBS += -L/usr/local/Cellar/boost/1.57.0/lib/ -lboost_thread-mt
macx: INCLUDEPATH += /usr/local/Cellar/boost/1.57.0/include
macx: DEPENDPATH += /usr/local/Cellar/boost/1.57.0/include

macx: LIBS += -L/usr/local/Cellar/gmp/6.0.0a/lib/ -lgmp
macx: INCLUDEPATH += /usr/local/Cellar/gmp/6.0.0a/include
macx: DEPENDPATH += /usr/local/Cellar/gmp/6.0.0a/include

macx: LIBS += -L/usr/local/Cellar/mpfr/3.1.2-p10/lib/ -lmpfr
macx: INCLUDEPATH += /usr/local/Cellar/mpfr/3.1.2-p10/include
macx: DEPENDPATH += /usr/local/Cellar/mpfr/3.1.2-p10/include

macx: LIBS += -L/usr/local/Cellar/cgal/4.5.2/lib/ -lCGAL
macx: INCLUDEPATH += /usr/local/Cellar/cgal/4.5.2/include
macx: DEPENDPATH += /usr/local/Cellar/cgal/4.5.2/include


RESOURCES += \
    icons.qrc

unix:!macx: QMAKE_CXXFLAGS += -frounding-math
unix:!macx: LIBS += -L/usr/lib/ -lGeographic -lCGAL -lgmp -lmpfr -lboost_system -lboost_thread
unix:!macx: INCLUDEPATH += /usr/include
unix:!macx: DEPENDPATH += /usr/include

DISTFILES +=
