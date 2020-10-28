TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES += PT_USE_GOTO=1

QMAKE_CXXFLAGS += -Weverything -Wno-gnu-label-as-value -Wno-gnu-designator

SOURCES += \
        main.c

HEADERS += \
    pt/pt.h
