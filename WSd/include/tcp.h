//*********************************************************************************************************************************
//
// PROJECT:							WSd (Weather Station - Daemon)
// FILE:								tcp class
// SUBSYSTEM:						main(...)
// LANGUAGE:						C++
// TARGET OS:						UNIX/LINUX/WINDOWS/MAC
// LIBRARY DEPENDANCE:	Qt
// NAMESPACE:						WSd
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2015, 2020 Gavin Blakeman.
//                      This file is part of the Weather Station - Daemon (WSd)
//
//                      WSd is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
//                      License as published by the Free Software Foundation, either version 2 of the License, or (at your option)
//                      any later version.
//
//                      WSd is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
//                      warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//                      more details.
//
//                      You should have received a copy of the GNU General Public License along with WSd.  If not,
//                      see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:            Implements the main(...) function
//
// HISTORY:             2015-05-17/GGB - Development of classes for WSd
//
//*********************************************************************************************************************************

#ifndef TCP_H
#define TCP_H

  // Standard C++ library header files

#include <cstdint>
#include <memory>

  // Miscellaneous library header files

#include <QtNetwork>

namespace WSd
{
  class CTCPSocket : public QTcpSocket
  {
    Q_OBJECT

  private:
    std::uint32_t siteID;
    std::uint32_t instrumentID;
    QString ipAddress;
    qint16 port;

  protected:

  public:
    CTCPSocket(QObject *parent, std::uint32_t  sid, std::uint32_t iid);

    bool readArchive();
    bool setTime();
    bool setInterval(std::uint8_t);
  };

}   // namespace WSd

#endif // TCP_H
