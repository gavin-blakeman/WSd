TEMPLATE=lib
CONFIG += staticlib create_prl

QT      += core network sql
QT      -= gui

QMAKE_CXXFLAGS += -std=c++17

DESTDIR = ""
OBJECTS_DIR = "objects"

HEADERS += \
  src/QtService \
  src/QtServiceBase \
  src/qtunixserversocket.h \
  src/qtunixsocket.h

SOURCES += \
  src/qtservice.cpp \
  src/qtservice_unix.cpp \
  src/qtunixserversocket.cpp \
  src/qtunixsocket.cpp

