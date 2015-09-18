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
    src/neuron.cpp \
    src/createxml.cpp \
    src/selectmodulatedvalues.cpp

HEADERS  += src/qnnvisualiser.h \
    src/networkrenderer.h \
    src/neuron.h \
    src/createxml.h \
    src/selectmodulatedvalues.h

FORMS    += src/qnnvisualiser.ui \
    src/createxml.ui \
    src/selectmodulatedvalues.ui

unix: LIBS += -L$$PWD/../qnn/ -lqnn
win32: LIBS += -L$$PWD/../qnn/ -lqnn0

INCLUDEPATH += $$PWD/../qnn/src
DEPENDPATH += $$PWD/../qnn/src


OTHER_FILES += \
    LICENSE.GPL3
