//*********************************************************************************************************************************
//
// PROJECT:							WSd (Weather Station - Daemon)
// FILE:								StateMachine
// SUBSYSTEM:						StateMachine
// LANGUAGE:						C++
// TARGET OS:						UNIX/LINUX/WINDOWS/MAC
// LIBRARY DEPENDANCE:	Qt
// NAMESPACE:						WSd:settings
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2015 Gavin Blakeman.
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
// OVERVIEW:            Implements the service
//
// HISTORY:             2015-05-17/GGB - Development of classes for WSd.
//
//*********************************************************************************************************************************

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include <cstdint>

#include <Qt>

#include "tcp.h"

namespace WSd
{

  class CStateMachine : public QObject
  {
    Q_OBJECT

  public:
    unsigned long siteID;
    unsigned long instrumentID;

  private:
    QObject *parent;
    //EPollState pollModeState;
    tcp::CTCPSocket *tcpSocket = nullptr;
    uint16_t recordsToFetch = 0;
    uint32_t lastJDReceived = 0;
    uint32_t lastSecReceived = 0;
    QTimer *pollTimer;

  protected:
  public:
    CStateMachine(QObject *, unsigned long site, unsigned long instrument);
    virtual ~CStateMachine();

    void start();
    void stop();

  public slots:
    void pollModeTimer();
  };


} // namespace OCWS


#endif // STATEMACHINE_H
