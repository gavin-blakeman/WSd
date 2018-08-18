﻿//*********************************************************************************************************************************
//
// PROJECT:							WSd (Weather Station - Daemon)
// FILE:								Service
// SUBSYSTEM:						Service
// LANGUAGE:						C++
// TARGET OS:						UNIX/LINUX/WINDOWS/MAC
// LIBRARY DEPENDANCE:	Qt
// NAMESPACE:						WSd
// AUTHOR:							Gavin Blakeman (GGB)
// LICENSE:             GPLv2
//
//                      Copyright 2014-2015 Gavin Blakeman.
//                      This file is part of the Observatory Control - Weather System (OCWS)
//
//                      OCWS is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
//                      License as published by the Free Software Foundation, either version 2 of the License, or (at your option)
//                      any later version.
//
//                      OCWS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
//                      warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//                      more details.
//
//                      You should have received a copy of the GNU General Public License along with OCWS.  If not,
//                      see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:            Implements the main(...) function
//
// HISTORY:             2014-07-19/GGB - Development of classes for OWS.
//
//*********************************************************************************************************************************

#ifndef SERVICE_H
#define SERVICE_H

#include <QtServiceBase>

#include "../Include/statemachine.h"

namespace WSd
{
  namespace service
  {
    class CWSService : public QObject, public QtService<QCoreApplication>
    {
      Q_OBJECT

    private:
      CStateMachine *stateMachine;
      unsigned long siteID;
      unsigned long instrumentID;

    protected:
      void start();
      void stop();
      void pause() {}
      void resume();

    public:
      CWSService(int argc, char **argv, unsigned long siteID, unsigned long instrumentID);
      virtual ~CWSService();
    };

  } // namespace service

}   // namespace WSd

#endif // SERVICE_H