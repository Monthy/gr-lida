# ############################################################################
# Qmake for building: gr-lida
# Description: GR-lida Front-End para DOSBox, ScummVM y el VDMSound
# Command: qmake gr-lida.pro && gmake
# ############################################################################
TEMPLATE = app

# ###### Output filename, directory
win32|mac {
    TARGET = GR-lida
} else {
    TARGET = gr-lida
}

DESTDIR += bin

# ###### Output directory
UI_DIR += src
MOC_DIR += build
RCC_DIR += build
OBJECTS_DIR += build

# ###### Compiler, tools and options
QT += core \
    gui \
    sql \
    network \
    script \
    phonon

CONFIG += qt \
    warn_on \
    thread \
    release

# CONFIG += qt warn_on thread debug
# ###### Files
INCLUDEPATH += . \
    src \
    ui \
    3rdparty

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
    src/grlida_addedit_dosbox.h \
    src/grlida_addedit_juego.h \
    src/grlida_addedit_montajes.h \
    src/grlida_addedit_scummvm.h \
    src/grlida_addedit_url.h \
    src/grlida_config_inicial.h \
    src/grlida_dbxadd.h \
    src/grlida_importar_juego.h \
    src/grlida_exportar_juego.h \
    src/grlida_importpath.h \
    src/grlida_opciones.h \
    src/grlida_ruleta.h \
    src/grlida_multimedia.h \
    src/grlida_svmadd.h \
    src/grlida_vdmsadd.h \
    src/stardelegate.h \
    src/httpdownload.h \
    src/qtzip.h \
    src/grlida_instalar_juego.h \
    src/grlida_addedit_vdmsound.h \
    3rdparty/crc32qt.h \
    src/grlida_pdf_viewer.h

SOURCES += src/grlida_img_viewer.cpp \
    src/grlida_info.cpp \
    src/dbsql.cpp \
    src/funciones.cpp \
    src/grlida.cpp \
    src/grlida_acercad.cpp \
    src/grlida_addedit_dosbox.cpp \
    src/grlida_addedit_juego.cpp \
    src/grlida_addedit_montajes.cpp \
    src/grlida_addedit_scummvm.cpp \
    src/grlida_addedit_url.cpp \
    src/grlida_config_inicial.cpp \
    src/grlida_dbxadd.cpp \
    src/grlida_importar_juego.cpp \
    src/grlida_importpath.cpp \
    src/grlida_exportar_juego.cpp \
    src/grlida_opciones.cpp \
    src/grlida_ruleta.cpp \
    src/grlida_multimedia.cpp \
    src/grlida_svmadd.cpp \
    src/grlida_vdmsadd.cpp \
    src/httpdownload.cpp \
    src/stardelegate.cpp \
    src/main.cpp \
    src/qtzip.cpp \
    src/grlida_instalar_juego.cpp \
    src/grlida_addedit_vdmsound.cpp \
    src/grlida_pdf_viewer.cpp

FORMS += ui/image_viewer.ui \
    ui/login_url.ui \
    ui/acercad.ui \
    ui/addedit_dosbox.ui \
    ui/addedit_juego.ui \
    ui/addedit_montajes.ui \
    ui/addedit_scummvm.ui \
    ui/addedit_url.ui \
    ui/configinicial.ui \
    ui/dbxadd.ui \
    ui/grlida.ui \
    ui/importar_juego.ui \
    ui/importpath.ui \
    ui/exportar_juego.ui \
    ui/opciones.ui \
    ui/ruleta.ui \
    ui/multimedia.ui \
    ui/svmadd.ui \
    ui/vdmsound.ui \
    ui/informacion.ui \
    ui/instalar_juego.ui \
    ui/addedit_vdmsound.ui \
    ui/pdf_viewer.ui

# ###### 3rdparty Files
HEADERS += 3rdparty/pictureflow-qt/pictureflow.h
SOURCES += 3rdparty/pictureflow-qt/pictureflow.cpp

HEADERS += 3rdparty/osdab-zip/zip.h \
        3rdparty/osdab-zip/zip_p.h \
        3rdparty/osdab-zip/unzip.h \
        3rdparty/osdab-zip/unzip_p.h \
        3rdparty/osdab-zip/zipentry_p.h
SOURCES += 3rdparty/osdab-zip/zip.cpp \
        3rdparty/osdab-zip/unzip.cpp

HEADERS += 3rdparty/doc_pdf_widget.h
SOURCES += 3rdparty/doc_pdf_widget.cpp
# ###### FIN 3rdparty

RESOURCES += res/grlida.qrc
TRANSLATIONS += lng/gr-lida_es_ES.ts \
    lng/gr-lida_en_EN.ts \
    lng/gr-lida_da_DK.ts \
    lng/gr-lida_fr_FR.ts \
    lng/gr-lida_hu_HU.ts \
    lng/gr-lida_ru_RU.ts

#CODECFORSRC = UTF-8

win32:RC_FILE += gr-lida.rc
mac:ICON += gr-lida.icns
mac|unix {
    LIBS += -lz
}
LIBS += -lpoppler-qt4

# ###### Install
!win32 {
    isEmpty(PREFIX):PREFIX = /usr/local
    DATADIR = $$PREFIX/share
    target.files = bin/gr-lida
    target.path = $$PREFIX/bin
    pixmaps.files = gr-lida.svg \
        gr-lida.png
    pixmaps.path = $$DATADIR/pixmaps
    desktop.files = res/gr-lida.desktop
    desktop.path = $$DATADIR/applications
    docs.files = doc/AUTHORS.txt \
        doc/COPYING.txt \
        doc/INSTALL.txt \
        doc/LEEME.txt \
        doc/LICENSE.txt \
        doc/NEWS.txt \
        doc/THANKS.txt \
        doc/TODO.txt
    docs.path = $$DATADIR/doc/GR-lida
    INSTALLS += target \
        pixmaps \
        desktop \
        docs
}

