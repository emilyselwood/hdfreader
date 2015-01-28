#-------------------------------------------------
#
# Project created by QtCreator 2015-01-25T00:17:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SMI
TEMPLATE = app
INCLUDEPATH += /home/matt/ESA/hdf-4.2.10/hdf4/include/
LIBS+= -L/home/matt/ESA/hdf-4.2.10/hdf4/lib/ -ldl -lm -lmfhdf -ldf -ljpeg -lz

SOURCES += main.cpp\
        mainwindow.cpp \
    widget.cpp

HEADERS  += mainwindow.h \
    widget.h

FORMS    += mainwindow.ui
