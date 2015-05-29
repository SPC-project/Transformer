QT += core gui

TARGET = Transformer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

INCLUDEPATH += ../include

SOURCES += ../src/main.cpp
SOURCES += ../src/Interface.cpp
SOURCES += ../src/Transformer.cpp

HEADERS += ../include/Interface.h
HEADERS += ../include/Transformer.h

FORMS = ../res/transgui.ui
