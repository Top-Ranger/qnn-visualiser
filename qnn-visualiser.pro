#-------------------------------------------------
#
# Project created by QtCreator 2015-09-17T17:54:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = qnn-visualiser
TEMPLATE = app


SOURCES += src/main.cpp\
        src/qnnvisualiser.cpp \
    src/networkrenderer.cpp \
    src/neuron.cpp

HEADERS  += src/qnnvisualiser.h \
    src/networkrenderer.h \
    src/neuron.h

FORMS    += src/qnnvisualiser.ui

OTHER_FILES += \
    LICENSE.GPL3
