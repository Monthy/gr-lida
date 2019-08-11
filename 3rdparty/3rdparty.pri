INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

#
HEADERS += $$PWD/editorwidget/codeeditor.h \
    $$PWD/editorwidget/editorwidget.h

SOURCES += $$PWD/editorwidget/codeeditor.cpp \
    $$PWD/editorwidget/editorwidget.cpp

#
HEADERS += $$PWD/pictureflow-qt/pictureflow.h
SOURCES += $$PWD/pictureflow-qt/pictureflow.cpp

#
HEADERS += $$PWD/doc_pdf_widget.h
SOURCES += $$PWD/doc_pdf_widget.cpp

#
HEADERS += $$PWD/animatedtextbrowser.h \
    $$PWD/crc32qt.h \
    $$PWD/findfiles.h \
    $$PWD/dockwidget.h

SOURCES += $$PWD/dockwidget.cpp

#
HEADERS += $$PWD/qcheckcombobox.h \
    $$PWD/qgroupcombobox.h
SOURCES += $$PWD/qcheckcombobox.cpp \
    $$PWD/qgroupcombobox.cpp

#
include($$PWD/propertyeditor/propertyeditor.pri)
include($$PWD/Qt7zip/Qt7zip.pri)
