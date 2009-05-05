TEMPLATE = app
INCLUDEPATH += . ../
QT -= qtgui
CONFIG += console debug
TARGET = zip

INCLUDEPATH += . ../

# Input
HEADERS += ../zip.h ../zip_p.h ../unzip.h ../unzip_p.h ../zipentry_p.h
SOURCES += main.cpp ../zip.cpp ../unzip.cpp
DESTDIR = bin
MOC_DIR = tmp
OBJECTS_DIR = tmp
