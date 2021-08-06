#-------------------------------------------------
#
# Project created by QtCreator 2021-07-11T20:26:18
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Zona_Roja_UO
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QXLSX_PARENTPATH=$$PWD/QXlsx/
QXLSX_HEADERPATH=$$PWD/QXlsx/header/
QXLSX_SOURCEPATH=$$PWD/QXlsx/source/
include($$PWD/QXlsx/QXlsx.pri)


SOURCES += \
        main.cpp \
        ventana_principal.cpp \
    pacientes.cpp \
    databasemanager.cpp \
    resultadospcr.cpp \
    actualizador.cpp \
    excelmanager.cpp \
    customtable.cpp

HEADERS += \
        ventana_principal.h \
    pacientes.h \
    databasemanager.h \
    resultadospcr.h \
    actualizador.h \
    excelmanager.h \
    customtable.h

FORMS += \
        ventana_principal.ui

RC_ICONS += 'icon.ico'

RESOURCES += \
    resources.qrc
