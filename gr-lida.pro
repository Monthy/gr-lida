# ############################################################################
# Qmake for building: gr-lida
# Description: GR-lida Front-End para DOSBox, ScummVM y el VDMSound
# Command: qmake gr-lida.pro && gmake
# ############################################################################

# ###### Output filename, directory
win32|mac {
    TARGET = GR-lida
} else {
    TARGET = gr-lida
}

TEMPLATE = app
DESTDIR += bin

# ###### Output directory
UI_DIR += src
MOC_DIR += build
RCC_DIR += build
OBJECTS_DIR += build

# ###### Compiler, tools and options
CONFIG += qt warn_on thread release

# Examples:
#CONFIG += qt warn_on thread release
#CONFIG += qt warn_on thread debug

QT += core gui sql network script phonon
#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# ###### Files
INCLUDEPATH += . \
    src \
    ui \
    3rdparty

DEPENDPATH += . \
    src \
    res \
    ui

HEADERS += src/dbsql.h \
    src/funciones.h \
    src/grlida.h \
    src/grlida_acercad.h \
    src/grlida_addedit_dosbox.h \
    src/grlida_addedit_juego.h \
    src/grlida_addedit_montajes.h \
    src/grlida_addedit_scummvm.h \
    src/grlida_addedit_vdmsound.h \
    src/grlida_config_inicial.h \
    src/grlida_dbxadd.h \
    src/grlida_delegate.h \
    src/grlida_exportar_juego.h \
    src/grlida_img_viewer.h \
    src/grlida_importar_juego.h \
    src/grlida_importpath.h \
    src/grlida_info.h \
    src/grlida_instalar_juego.h \
    src/grlida_list_icon_cfg.h \
    src/grlida_multimedia.h \
    src/grlida_opciones.h \
    src/grlida_pdf_viewer.h \
    src/grlida_picflow.h \
    src/grlida_ruleta.h \
    src/grlida_svmadd.h \
    src/grlida_update.h \
    src/grlida_vdmsadd.h \
    src/g_structs.h \
    src/httpdownload.h \
    src/qtzip.h

SOURCES += src/dbsql.cpp \
    src/funciones.cpp \
    src/grlida.cpp \
    src/grlida_acercad.cpp \
    src/grlida_addedit_dosbox.cpp \
    src/grlida_addedit_juego.cpp \
    src/grlida_addedit_montajes.cpp \
    src/grlida_addedit_scummvm.cpp \
    src/grlida_addedit_vdmsound.cpp \
    src/grlida_config_inicial.cpp \
    src/grlida_dbxadd.cpp \
    src/grlida_delegate.cpp \
    src/grlida_exportar_juego.cpp \
    src/grlida_img_viewer.cpp \
    src/grlida_importar_juego.cpp \
    src/grlida_importpath.cpp \
    src/grlida_info.cpp \
    src/grlida_instalar_juego.cpp \
    src/grlida_list_icon_cfg.cpp \
    src/grlida_multimedia.cpp \
    src/grlida_opciones.cpp \
    src/grlida_pdf_viewer.cpp \
    src/grlida_picflow.cpp \
    src/grlida_ruleta.cpp \
    src/grlida_svmadd.cpp \
    src/grlida_update.cpp \
    src/grlida_vdmsadd.cpp \
    src/httpdownload.cpp \
    src/main.cpp\
    src/qtzip.cpp

FORMS += ui/grlida.ui \
    ui/opciones.ui \
    ui/addedit_juego.ui \
    ui/addedit_vdmsound.ui \
    ui/addedit_scummvm.ui \
    ui/addedit_dosbox.ui \
    ui/acercad.ui \
    ui/importar_juego.ui \
    ui/login_url.ui \
    ui/importpath.ui \
    ui/addedit_montajes.ui \
    ui/dbxadd.ui \
    ui/svmadd.ui \
    ui/vdmsound.ui \
    ui/config_inicial.ui \
    ui/ruleta.ui \
    ui/img_viewer.ui \
    ui/instalar_juego.ui \
    ui/pdf_viewer.ui \
    ui/multimedia_setup.ui \
    ui/informacion.ui \
    ui/update.ui \
    ui/exportar_juego.ui \
    ui/list_icon_cfg.ui

# ###### 3rdparty Files
HEADERS += 3rdparty/pictureflow-qt/pictureflow.h
SOURCES += 3rdparty/pictureflow-qt/pictureflow.cpp

HEADERS += 3rdparty/osdab-zip/unzip.h \
    3rdparty/osdab-zip/unzip_p.h \
    3rdparty/osdab-zip/zip.h \
    3rdparty/osdab-zip/zipentry_p.h \
    3rdparty/osdab-zip/zipglobal.h \
    3rdparty/osdab-zip/zip_p.h

SOURCES += 3rdparty/osdab-zip/unzip.cpp \
    3rdparty/osdab-zip/zip.cpp \
    3rdparty/osdab-zip/zipglobal.cpp

HEADERS += 3rdparty/doc_pdf_widget.h
SOURCES += 3rdparty/doc_pdf_widget.cpp

HEADERS += 3rdparty/crc32qt.h

HEADERS += 3rdparty/qcheckcombobox.h
SOURCES += 3rdparty/qcheckcombobox.cpp
# ###### FIN 3rdparty

RESOURCES += res/gr-lida.qrc

TRANSLATIONS += res/idiomas/gr-lida_es_ES.ts \
    res/idiomas/gr-lida_en_EN.ts \
    res/idiomas/gr-lida_da_DK.ts \
    res/idiomas/gr-lida_fr_FR.ts \
    res/idiomas/gr-lida_hu_HU.ts \
    res/idiomas/gr-lida_ru_RU.ts

#CODECFORTR  = UTF-8
#CODECFORSRC = UTF-8

win32:RC_FILE += gr-lida.rc
mac:ICON += gr-lida.icns

LIBS += -lz -lpoppler-qt4

# ###### Install
!win32 {
    target.files = bin/gr-lida
    target.path = /usr/bin
    pixmaps.files = gr-lida.svg \
        gr-lida.png
    pixmaps.path = /usr/share/pixmaps
    desktop.files = res/gr-lida.desktop
    desktop.path = /usr/share/applications
    docs.files = doc/AUTHORS.txt \
        doc/COPYING.txt \
        doc/INSTALL.txt \
        doc/LEEME.txt \
        doc/LICENSE.txt \
        doc/NEWS.txt \
        doc/THANKS.txt \
        doc/TODO.txt
    docs.path = /usr/share/doc/gr-lida
    INSTALLS += target \
        pixmaps \
        desktop \
        docs
}
