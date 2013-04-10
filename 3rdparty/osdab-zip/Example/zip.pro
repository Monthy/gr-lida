TEMPLATE = app
INCLUDEPATH += . ../
QT -= gui
CONFIG += console debug
TARGET = zip

INCLUDEPATH += . ../

# Input
HEADERS += ../zipglobal.h ../zip.h ../zip_p.h ../unzip.h ../unzip_p.h ../zipentry_p.h
SOURCES += main.cpp ../zipglobal.cpp ../zip.cpp ../unzip.cpp
DESTDIR = bin
MOC_DIR = tmp
OBJECTS_DIR = tmp
