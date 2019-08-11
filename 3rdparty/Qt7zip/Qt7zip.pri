INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QMAKE_CFLAGS += -w
QMAKE_CXXFLAGS += -w
DEFINES += _UNICODE _LARGEFILE_SOURCE _7ZIP_LARGE_PAGES

win32 {
    7ZIP_BASE=$$PWD/7zip_src

    DEFINES += WIN_LONG_PATH

    LIBS += -loleaut32 -luser32 -luuid
}

unix {
    7ZIP_BASE=$$PWD/p7zip_src

    macx:DEFINES += ENV_MACOSX _TCHAR_DEFINED
    DEFINES += UNICODE _FILE_OFFSET_BITS=64 NDEBUG _REENTRANT ENV_UNIX _7ZIP_ST UNIX_USE_WIN_FILE
    DEFINES += "LIBDIR=\\\"$$/usr/lib\\\""

    SOURCES += $$7ZIP_BASE/CPP/myWindows/wine_date_and_time.cpp

    INCLUDEPATH += $$7ZIP_BASE/CPP/myWindows \
        $$7ZIP_BASE/CPP/include_windows
}

INCLUDEPATH += $$7ZIP_BASE \
    $$7ZIP_BASE/C \
    $$7ZIP_BASE/CPP \
    $$7ZIP_BASE/CPP/Common

# 7-Zip comun --
SOURCES += $$7ZIP_BASE/CPP/7zip/Common/FileStreams.cpp \
    $$7ZIP_BASE/CPP/7zip/Common/StreamObjects.cpp \
    $$7ZIP_BASE/CPP/7zip/Common/StreamUtils.cpp \
    $$7ZIP_BASE/CPP/Common/IntToString.cpp \
    $$7ZIP_BASE/CPP/Common/MyString.cpp \
    $$7ZIP_BASE/CPP/Common/MyVector.cpp \
    $$7ZIP_BASE/CPP/Common/MyWindows.cpp \
    $$7ZIP_BASE/CPP/Common/StringConvert.cpp \
    $$7ZIP_BASE/CPP/Common/Wildcard.cpp \
    $$7ZIP_BASE/CPP/Windows/FileDir.cpp \
    $$7ZIP_BASE/CPP/Windows/FileFind.cpp \
    $$7ZIP_BASE/CPP/Windows/FileIO.cpp \
    $$7ZIP_BASE/CPP/Windows/FileName.cpp \
    $$7ZIP_BASE/CPP/Windows/PropVariant.cpp \
    $$7ZIP_BASE/CPP/Windows/PropVariantConv.cpp \
    $$7ZIP_BASE/C/Alloc.c \
    $$7ZIP_BASE/C/Threads.c

HEADERS += $$7ZIP_BASE/CPP/7zip/Common/FileStreams.h \
    $$7ZIP_BASE/CPP/7zip/Common/StreamObjects.h \
    $$7ZIP_BASE/CPP/7zip/Common/StreamUtils.h \
    $$7ZIP_BASE/CPP/7zip/ICoder.h \
    $$7ZIP_BASE/CPP/7zip/IStream.h \
    $$7ZIP_BASE/CPP/Common/IntToString.h \
    $$7ZIP_BASE/CPP/Common/MyString.h \
    $$7ZIP_BASE/CPP/Common/MyVector.h \
    $$7ZIP_BASE/CPP/Common/MyWindows.h \
    $$7ZIP_BASE/CPP/Common/StringConvert.h \
    $$7ZIP_BASE/CPP/Common/Wildcard.h \
    $$7ZIP_BASE/CPP/Windows/FileDir.h \
    $$7ZIP_BASE/CPP/Windows/FileFind.h \
    $$7ZIP_BASE/CPP/Windows/FileIO.h \
    $$7ZIP_BASE/CPP/Windows/FileName.h \
    $$7ZIP_BASE/CPP/Windows/PropVariant.h \
    $$7ZIP_BASE/CPP/Windows/PropVariantConv.h \
    $$7ZIP_BASE/C/Alloc.h \
    $$7ZIP_BASE/C/Threads.h

# Qt7Zip wrapper --
HEADERS += $$PWD/qt7zip.h \
    $$PWD/7zip_includes.h \
    $$PWD/open_callback.h \
    $$PWD/extract_callback.h \
    $$PWD/extract_callback_raw.h \
    $$PWD/creating_callback.h \
    $$PWD/compress_codecs_info.h

SOURCES += $$PWD/qt7zip.cpp
