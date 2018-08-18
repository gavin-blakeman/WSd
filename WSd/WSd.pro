##**********************************************************************************************************************************
#
## PROJECT:							WSd (Weather Station - Daemon)
## FILE:								main application
## SUBSYSTEM:						main(...)
## LANGUAGE:						C++
## TARGET OS:						UNIX/LINUX/WINDOWS/MAC
## LIBRARY DEPENDANCE:	Qt
## NAMESPACE:						WS
## AUTHOR:							Gavin Blakeman (GGB)
## LICENSE:             GPLv2
##
##                      Copyright 2015, 2017 Gavin Blakeman.
##                      This file is part of the Weather Station - Daemon (WSd)
##
##                      WSd is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
##                      License as published by the Free Software Foundation, either version 2 of the License, or (at your option)
##                      any later version.
##
##                      WSd is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
##                      warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
##                      more details.
##
##                      You should have received a copy of the GNU General Public License along with WSd.  If not,
##                      see <http://www.gnu.org/licenses/>.
##
## OVERVIEW:            Project File.
##
## HISTORY:             2015-05-17/GGB - Development of classes for WSd.
##
##**********************************************************************************************************************************

QT       += core network sql
QT       -= gui

TEMPLATE = app
TARGET = WSd
CONFIG   += console qt thread static

QMAKE_CXXFLAGS += -std=c++17 -static -static-libgcc

DEFINES += BOOST_THREAD_USE_LIB
DEFINES += QT_CORE_LIB
DEFINES += QXT_STATIC
DEFINES += BOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0

INCLUDEPATH +=  "../qt-solutions" \
                "../ACL" \
                "../boost 1.62" \
                "../cfitsio" \
                "../GCL" \
                "../MCL" \
                "../PCL" \
                "../SCL" \
                "../WCL"

SOURCES += \
    Source/service.cpp \
    Source/statemachine.cpp \
    ../qt-solutions/qtservice/src/qtunixsocket.cpp \
    ../qt-solutions/qtservice/src/qtunixserversocket.cpp \
    ../qt-solutions/qtservice/src/qtservice_unix.cpp \
    ../qt-solutions/qtservice/src/qtservice.cpp \
    Source/tcp.cpp \
    ../WCL/source/GeneralFunctions.cpp \
    ../WCL/source/database.cpp \
    ../WCL/source/error.cpp \
    ../WCL/source/settings.cpp \
    Source/WSd.cpp

HEADERS += \
    Include/service.h \
    Include/statemachine.h \
    Include/tcp.h \
    ../qt-solutions/qtservice/src/qtunixsocket.h \
    ../qt-solutions/qtservice/src/qtunixserversocket.h \
    ../qt-solutions/qtservice/src/qtservice_p.h \
    ../qt-solutions/qtservice/src/qtservice.h \
    ../WCL/include/WeatherLinkIP.h \
    ../WCL/include/GeneralFunctions.h \
    ../WCL/include/database.h \
    ../WCL/include/settings.h

win32:CONFIG(release, debug|release) {
  LIBS += -L../../Library/Library/win32/release/ -lGCL
}
else:win32:CONFIG(debug, debug|release) {
  LIBS += -L../../Library/Library/win32/debug -lACL
  LIBS += -L../../Library/Library/win32/debug -lGCL
  LIBS += -L../../Library/Library/win32/debug -lPCL
  LIBS += -L../../Library/Library/win32/debug -lQwt
  LIBS += -L../../Library/Library/win32/debug -lSCL
  LIBS += -L../../Library/Library/win32/debug -lboost_filesystem
  LIBS += -L../../Library/Library/win32/debug -lboost_system
  LIBS += -L../../Library/Library/win32/debug -lboost_thread
  LIBS += -L../../Library/Library/win32/debug -lQxt
}
else:unix:CONFIG(debug, debug|release) {
  LIBS += -L../../Library/Library/unix/debug -lACL
  LIBS += -L../../Library/Library/unix/debug -lGCL
  LIBS += -L../../Library/Library/unix/debug -lPCL
  #LIBS += -L../../Library/Library/unix/debug -lSCL
  LIBS += -L../../Library/Library -lSOFA
  LIBS += -L../../../Library/Library/unix/debug -lboost_chrono
  LIBS += -L../../../Library/Library/unix/debug -lboost_filesystem
  LIBS += -L../../../Library/Library/unix/debug -lboost_program_options
  LIBS += -L../../../Library/Library/unix/debug -lboost_system
  LIBS += -L../../../Library/Library/unix/debug -lboost_thread
  LIBS += -L../../../Library/Library -lQxt
}
else:unix:CONFIG(release, debug|release) {
  LIBS += -L../../Library/Library/unix/release -lACL
  LIBS += -L../../../Library/Library/unix/release -lGCL
  LIBS += -L../../../Library/Library/unix/release -lPCL
  LIBS += -L../../../Library/Library/unix/release -lSCL
  LIBS += -L../../Library/Library -lSOFA
  LIBS += -L../../../Library/Library/unix/release -lboost_chrono
  LIBS += -L../../../Library/Library/unix/release -lboost_filesystem
  LIBS += -L../../../Library/Library/unix/release -lboost_program_options
  LIBS += -L../../../Library/Library/unix/release -lboost_system
  LIBS += -L../../../Library/Library/unix/release -lboost_thread
  LIBS += -L../../../Library/Library/unix/release -lQxt
}

OTHER_FILES += \
    README.txt
