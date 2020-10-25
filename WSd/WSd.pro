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
##                      Copyright 2015, 2019-2020 Gavin Blakeman.
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
## HISTORY:             2019-10-05/GGB - Changed compiler to use C++17.
##											2015-05-17/GGB - Development of classes for WSd.
##
##**********************************************************************************************************************************

TEMPLATE = app
TARGET = WSd
CONFIG   += console qt thread static link_prl

QT       += core network sql
QT       -= gui

QMAKE_CXXFLAGS += -std=c++17 -static -static-libgcc

DEFINES += BOOST_THREAD_USE_LIB
DEFINES += QT_CORE_LIB
DEFINES += QXT_STATIC
DEFINES += BOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0

OBJECTS_DIR = "objects"
MOC_DIR = "moc"

INCLUDEPATH +=  \
  "../qtservice/src" \
  "../ACL" \
  "/home/gavin/Documents/Projects/software/Library/Boost/boost_1_71_0" \
  "../GCL" \
  "../MCL" \
  "../PCL" \
  "../SCL" \
  "../QCL" \
  "../WCL" \
  "../LibRaw" \
  "../cfitsio"

SOURCES += \
    source/WSD.cpp \
    source/service.cpp \
    source/statemachine.cpp \
    source/tcp.cpp \

HEADERS += \
    include/service.h \
    include/statemachine.h \
    include/tcp.h \

win32:CONFIG(release, debug|release) {
  LIBS += -L../../Library/Library/win32/release/ -lGCL
}
else:win32:CONFIG(debug, debug|release) {
  LIBS += -L../../Library/Library/win32/debug -lACL
  LIBS += -L../../Library/Library/win32/debug -lGCL
  LIBS += -L../../Library/Library/win32/debug -lPCL
  LIBS += -L../../Library/Library/win32/debug -lSCL
  LIBS += -L../../Library/Library/win32/debug -lboost_filesystem
  LIBS += -L../../Library/Library/win32/debug -lboost_system
  LIBS += -L../../Library/Library/win32/debug -lboost_thread
  LIBS += -L../../Library/Library/win32/debug -lQxt
}
else:unix:CONFIG(debug, debug|release) {
  LIBS += -L../ACL -lACL
  LIBS += -L../GCL -lGCL
  LIBS += -L../PCL -lPCL
  LIBS += -L../SCL -lSCL
  LIBS += -L../QCL -lQCL
  LIBS += -L../WCL -lWCL
  LIBS += -L../SOFA -lSOFA
  LIBS += -L../qtservice -lqtservice
  LIBS += -L/usr/local/lib -lboost_chrono
  LIBS += -L/usr/local/lib -lboost_filesystem
  LIBS += -L/usr/local/lib -lboost_program_options
  LIBS += -L/usr/local/lib -lboost_system
  LIBS += -L/usr/local/lib -lboost_thread
  LIBS += -L/usr/local/lib -lboost_locale
}
else:unix:CONFIG(release, debug|release) {
  LIBS += -L../ACL -lACL
  LIBS += -L../GCL -lGCL
  LIBS += -L../PCL -lPCL
  LIBS += -L../SCL -lSCL
  LIBS += -L../QCL -lQCL
  LIBS += -L../WCL -lWCL
  LIBS += -L../SOFA -lSOFA
  LIBS += -L../qtservice -lqtservice
  LIBS += -L/usr/local/lib -lboost_chrono
  LIBS += -L/usr/local/lib -lboost_filesystem
  LIBS += -L/usr/local/lib -lboost_program_options
  LIBS += -L/usr/local/lib -lboost_system
  LIBS += -L/usr/local/lib -lboost_thread
  LIBS += -L/usr/local/lib -lboost_locale
}

OTHER_FILES += \
    README.txt
