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
#LANGUAGE = C++

# ###### Compiler, tools and options
CONFIG += qt warn_on thread release
CONFIG += c++11

# ###### Output directory
DESTDIR += bin
UI_DIR += build/ui
MOC_DIR += build/moc
RCC_DIR += build/rcc
OBJECTS_DIR += build/obj

# Examples:
#CONFIG += qt warn_on thread release
#CONFIG += qt warn_on thread debug
# phonon
QT += core gui sql network script

contains(QT_CONFIG, opengl): QT += opengl

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets printsupport
    TARGET_ARCH = $${QT_ARCH}

    message(Building with Qt5 $${TARGET_ARCH})
} else {
    TARGET_ARCH = $${QMAKE_HOST.arch}

    message(Building with Qt4 $${TARGET_ARCH})
}

contains(TARGET_ARCH, x86_64) {
    TARGET_ARCH = x64
} else {
    TARGET_ARCH = x86
}

# ###### Files
INCLUDEPATH += . \
    src \
    ui \
    3rdparty

DEPENDPATH += . \
    src \
    res \
    ui

HEADERS += src/grlida.h \
    src/grlida_picflow.h \
    src/funciones.h \
    src/g_structs.h \
    src/grlida_delegate.h \
    src/dbsql.h \
    src/grlida_info.h \
    src/grlida_acercad.h \
    src/grlida_config_inicial.h \
    src/grlida_buscar.h \
    src/grlida_list_icon_cfg.h \
	src/grlida_listview.h \
    src/grlida_addedit_juego.h \
    src/grlida_multimedia.h \
    src/grlida_opciones.h \
    src/grlida_addedit_scummvm.h \
    src/grlida_addedit_vdmsound.h \
    src/grlida_addedit_dosbox.h \
    src/grlida_addedit_montajes.h \
    src/grlida_addedit_virtual_drive.h \
    src/grlida_wizard_dosbox.h \
    src/grlida_wizard_scummvm.h \
    src/grlida_wizard_vdmsound.h \
    src/grlida_instalar_juego.h \
    src/grlida_cambiar_categoria.h \
    src/grlida_compatibilidad_exe.h \
    src/grdap.h \
    src/grdap_acercad.h \
    src/grdapview.h \
    src/grlida_pdf_viewer.h \
    src/grlida_importar_juego.h \
    src/grlida_importar_datos.h \
    src/grlida_importar_dosbox.h \
    src/grlida_importar_scummvm.h \
    src/httpdownload.h \
    src/grlida_update.h

SOURCES += src/main.cpp \
    src/grlida.cpp \
    src/grlida_picflow.cpp \
    src/funciones.cpp \
    src/grlida_delegate.cpp \
    src/dbsql.cpp \
    src/grlida_info.cpp \
    src/grlida_acercad.cpp \
    src/grlida_config_inicial.cpp \
    src/grlida_buscar.cpp \
    src/grlida_list_icon_cfg.cpp \
	src/grlida_listview.cpp \
    src/grlida_addedit_juego.cpp \
    src/grlida_multimedia.cpp \
    src/grlida_opciones.cpp \
    src/grlida_addedit_scummvm.cpp \
    src/grlida_addedit_vdmsound.cpp \
    src/grlida_addedit_dosbox.cpp \
    src/grlida_addedit_montajes.cpp \
    src/grlida_addedit_virtual_drive.cpp \
    src/grlida_wizard_dosbox.cpp \
    src/grlida_wizard_scummvm.cpp \
    src/grlida_wizard_vdmsound.cpp \
    src/grlida_instalar_juego.cpp \
    src/grlida_cambiar_categoria.cpp \
    src/grlida_compatibilidad_exe.cpp \
    src/grdap.cpp \
    src/grdap_acercad.cpp \
    src/grdapview.cpp \
    src/grlida_pdf_viewer.cpp \
    src/grlida_importar_juego.cpp \
    src/grlida_importar_datos.cpp \
    src/grlida_importar_dosbox.cpp \
    src/grlida_importar_scummvm.cpp \
    src/httpdownload.cpp \
    src/grlida_update.cpp

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += multimedia multimediawidgets

    SOURCES += src/grlida_media_qt5.cpp
    HEADERS += src/grlida_media_qt5.h

} else {
    QT += phonon

    SOURCES += src/grlida_media.cpp
    HEADERS += src/grlida_media.h
}

FORMS += ui/grlida.ui \
    ui/informacion.ui \
    ui/acercad.ui \
    ui/config_inicial.ui \
    ui/buscar.ui \
    ui/list_icon_cfg.ui \
    ui/addedit_juego.ui \
    ui/opciones.ui \
    ui/addedit_scummvm.ui \
    ui/addedit_vdmsound.ui \
    ui/addedit_dosbox.ui \
    ui/addedit_montajes.ui \
    ui/addedit_virtual_drive.ui \
    ui/instalar_juego.ui \
    ui/cambiar_categoria.ui \
    ui/compatibilidad_exe.ui \
    ui/wizard_dosbox.ui \
    ui/wizard_scummvm.ui \
    ui/wizard_vdmsound.ui \
    ui/grdap.ui \
    ui/grdap_acercad.ui \
    ui/pdf_viewer.ui \
    ui/importar_juego.ui \
    ui/importar_datos.ui \
    ui/importar_dosbox.ui \
    ui/importar_scummvm.ui \
    ui/login_url.ui \
    ui/update.ui

# ###### 3rdparty Files
include(3rdparty/3rdparty.pri)

RESOURCES += res/gr-lida.qrc

win32:RC_FILE += gr-lida.rc
mac:ICON += gr-lida.icns

greaterThan(QT_MAJOR_VERSION, 4) {
    DEFINES += IS_POPPLER_QT5
    LIBS += -lpoppler-qt5
} else {
    LIBS += -lpoppler-qt4
}

#CODECFORTR  = UTF-8
#CODECFORSRC = UTF-8

TRANSLATIONS += res/idiomas/gr-lida_es_ES.ts \
    res/idiomas/gr-lida_en_EN.ts \
    res/idiomas/gr-lida_da_DK.ts \
    res/idiomas/gr-lida_fr_FR.ts \
    res/idiomas/gr-lida_hu_HU.ts \
    res/idiomas/gr-lida_ru_RU.ts

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
    docs.path = $$DATADIR/doc/gr-lida
    INSTALLS += target \
        pixmaps \
        desktop \
        docs
}
