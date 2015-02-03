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
    src/exceptions/grid_exception.cpp \
    src/services/grid_service.cpp \
    src/services/project_service.cpp \
    src/ui/main_window.cpp \
    src/ui/new_project_dialog.cpp \
    src/ui/project_properties_dialog.cpp \
    src/ui/structured_grid_dialog.cpp \
    src/ui/unstructured_grid_dialog.cpp \
    src/utility/database_utility.cpp

HEADERS  += \
    include/domain/project.h \
    include/application/iph_application.h \
    include/dao/project_dao.h \
    include/exceptions/database_exception.h \
    include/exceptions/grid_exception.h \
    include/services/grid_service.h \
    include/services/project_service.h \
    include/ui/main_window.h \
    include/ui/new_project_dialog.h \
    include/ui/project_properties_dialog.h \
    include/ui/structured_grid_dialog.h \
    include/ui/unstructured_grid_dialog.h \
    include/utility/database_utility.h

FORMS    += include/ui/mainwindow.ui \
    include/ui/newprojectdialog.ui \
    include/ui/projectpropertiesdialog.ui \
    include/ui/structuredgriddialog.ui \
    include/ui/unstructuredgriddialog.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/lib/release/ -lGeographic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/lib/debug/ -lGeographic
else:unix: LIBS += -L$$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/lib/ -lGeographic

INCLUDEPATH += $$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/include
DEPENDPATH += $$PWD/../../../../../usr/local/Cellar/geographiclib/1.40/include
