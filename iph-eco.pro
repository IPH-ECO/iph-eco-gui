#-------------------------------------------------
#
# Project created by QtCreator 2015-01-16T00:31:49
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iph-eco
TEMPLATE = app


SOURCES += src/ui/mainwindow.cpp \
    src/ui/newprojectdialog.cpp \
    src/ui/projectpropertiesdialog.cpp \
    src/ui/structuredgriddialog.cpp \
    src/ui/unstructuredgriddialog.cpp \
    src/main.cpp \
    src/domain/project.cpp \
    src/services/projectservice.cpp \
    src/utility/databaseutility.cpp \
    src/exceptions/databaseexception.cpp \
    src/dao/projectdao.cpp \
    src/application/iphapplication.cpp \
    src/services/gridservice.cpp \
    src/exceptions/gridexception.cpp

HEADERS  += include/ui/mainwindow.h \
    include/ui/newprojectdialog.h \
    include/ui/projectpropertiesdialog.h \
    include/ui/structuredgriddialog.h \
    include/ui/unstructuredgriddialog.h \
    include/domain/project.h \
    include/services/projectservice.h \
    include/utility/databaseutility.h \
    include/exceptions/databaseexception.h \
    include/dao/projectdao.h \
    include/application/iphapplication.h \
    include/services/gridservice.h \
    include/exceptions/gridexception.h

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
