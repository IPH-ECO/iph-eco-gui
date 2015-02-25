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
    src/ui/structured_grid_dialog.cpp \
    src/ui/unstructured_grid_dialog.cpp \
    src/utility/database_utility.cpp \
    src/exceptions/mesh_exception.cpp \
    src/domain/unstructured_mesh.cpp \
    src/domain/structured_mesh.cpp \
    src/domain/mesh.cpp \
    src/domain/observers/observable.cpp \
    src/domain/observers/observer.cpp \
    src/ui/unstructured_mesh_dialog.cpp \
    src/ui/unstructured_mesh_opengl_widget.cpp \
    src/domain/mesh_polygon.cpp

HEADERS  += \
    include/domain/project.h \
    include/application/iph_application.h \
    include/dao/project_dao.h \
    include/exceptions/database_exception.h \
    include/services/project_service.h \
    include/ui/main_window.h \
    include/ui/new_project_dialog.h \
    include/ui/project_properties_dialog.h \
    include/ui/structured_grid_dialog.h \
    include/ui/unstructured_grid_dialog.h \
    include/utility/database_utility.h \
    include/exceptions/mesh_exception.h \
    include/domain/unstructured_mesh.h \
    include/domain/structured_mesh.h \
    include/domain/mesh.h \
    include/domain/observers/observable.h \
    include/domain/observers/observer.h \
    include/domain/observers/mesh_observer.h \
    include/domain/observers/mesh_observable.h \
    include/ui/unstructured_mesh_dialog.h \
    include/ui/unstructured_mesh_opengl_widget.h \
    include/utility/delaunay_triangulation_definitions.h \
    include/domain/mesh_polygon.h

FORMS    += \
    include/ui/main_window.ui \
    include/ui/new_project_dialog.ui \
    include/ui/project_properties_dialog.ui \
    include/ui/structured_grid_dialog.ui \
    include/ui/unstructured_grid_dialog.ui \
    include/ui/unstructured_mesh_dialog.ui

QMAKE_CXXFLAGS += -Wredeclared-class-member

INCLUDEPATH += /usr/local/Cellar/boost/1.57.0/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/lib/release/ -lGeographic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/lib/debug/ -lGeographic
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/lib/ -lGeographic

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/cgal/4.5/lib/release/ -lCGAL
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/cgal/4.5/lib/debug/ -lCGAL
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/cgal/4.5/lib/ -lCGAL

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/cgal/4.5/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/cgal/4.5/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/gmp/6.0.0a/lib/release/ -lgmp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/gmp/6.0.0a/lib/debug/ -lgmp
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/gmp/6.0.0a/lib/ -lgmp

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/gmp/6.0.0a/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/gmp/6.0.0a/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/mpfr/3.1.2-p10/lib/release/ -lmpfr
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/mpfr/3.1.2-p10/lib/debug/ -lmpfr
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/mpfr/3.1.2-p10/lib/ -lmpfr

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/mpfr/3.1.2-p10/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/mpfr/3.1.2-p10/include
