#-------------------------------------------------
#
# Project created by QtCreator 2015-01-16T00:31:49
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = iph-eco
TEMPLATE = app


SOURCES += src/ui/mainwindow.cpp \
    src/ui/newprojectdialog.cpp \
    src/ui/projectpropertiesdialog.cpp \
    src/ui/structuredgriddialog.cpp \
    src/ui/unstructuredgriddialog.cpp \
    src/main.cpp \
    src/domain/project.cpp

HEADERS  += include/ui/mainwindow.h \
    include/ui/newprojectdialog.h \
    include/ui/projectpropertiesdialog.h \
    include/ui/structuredgriddialog.h \
    include/ui/unstructuredgriddialog.h \
    include/domain/project.h

FORMS    += include/ui/mainwindow.ui \
    include/ui/newprojectdialog.ui \
    include/ui/projectpropertiesdialog.ui \
    include/ui/structuredgriddialog.ui \
    include/ui/unstructuredgriddialog.ui
