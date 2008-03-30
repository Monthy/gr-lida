# #####################################################################
# Qmake for building: gr-lida
# Description: GR-lida Front-End para DOSBox, ScummVM y el VDMSound
# Command: qmake gr-lida.pro && gmake
# #####################################################################
TEMPLATE = app

# ###### Output filename, directory
win32: TARGET = GR-lida
!win32: TARGET = gr-lida
DESTDIR += ./bin

# ###### Compiler, tools and options
UI_DIR += ./src/
MOC_DIR += ./build/
RCC_DIR += ./build/
OBJECTS_DIR += ./build/
QT += core \
    gui \
    sql \
    xml \
    network
CONFIG += qt \
    warn_on \
    thread \
    release

# CONFIG += qt warn_on thread debug
INCLUDEPATH += . \
    src \
    ui
DEPENDPATH += . \
    src \
    lng \
    res \
    ui
HEADERS += src/grlida_img_viewer.h \
    src/grlida_info.h \
    src/dbsql.h \
    src/funciones.h \
    src/grlida.h \
    src/grlida_acercad.h \
    src/grlida_addedit_juego.h \
    src/grlida_addedit_montajes.h \
    src/grlida_addedit_url.h \
    src/grlida_config_inicial.h \
    src/grlida_dbxadd.h \
    src/grlida_importar_juego.h \
    src/grlida_importpath.h \
    src/grlida_opciones.h \
    src/grlida_svmadd.h \
    src/grlida_vdmsadd.h

SOURCES += src/grlida_img_viewer.cpp \
    src/grlida_info.cpp \
    src/dbsql.cpp \
    src/funciones.cpp \
    src/grlida.cpp \
    src/grlida_acercad.cpp \
    src/grlida_addedit_juego.cpp \
    src/grlida_addedit_montajes.cpp \
    src/grlida_addedit_url.cpp \
    src/grlida_config_inicial.cpp \
    src/grlida_dbxadd.cpp \
    src/grlida_importar_juego.cpp \
    src/grlida_importpath.cpp \
    src/grlida_opciones.cpp \
    src/grlida_svmadd.cpp \
    src/grlida_vdmsadd.cpp \
    src/main.cpp

FORMS += ui/login_url.ui \
    ui/acercad.ui \
    ui/addedit_juego.ui \
    ui/addedit_montajes.ui \
    ui/addedit_url.ui \
    ui/configinicial.ui \
    ui/dbxadd.ui \
    ui/grlida.ui \
    ui/importar_juego.ui \
    ui/importpath.ui \
    ui/opciones.ui \
    ui/svmadd.ui \
    ui/vdmsound.ui \
    ui/informacion.ui

RESOURCES += res/grlida.qrc
TRANSLATIONS += lng/gr-lida_es_ES.ts \
    lng/gr-lida_en_EN.ts
	
win32:RC_FILE += gr-lida.rc
mac:ICON += gr-lida.icns
macx:ICON += gr-lida.icns

# ###### Install
!win32 { 
    target.files = bin/gr-lida
    target.path = /usr/bin
    pixmaps.files = gr-lida.svg gr-lida.png
    pixmaps.path = /usr/share/pixmaps
    desktop.files = res/gr-lida.desktop
    desktop.path = /usr/share/applications
    docs.files = doc/AUTHORS.txt doc/COPYING.txt doc/INSTALL.txt doc/LEEME.txt doc/LICENSE.txt doc/NEWS.txt doc/THANKS.txt doc/TODO.txt
    docs.path = /usr/share/doc/GR-lida

    INSTALLS += target pixmaps desktop docs
}
