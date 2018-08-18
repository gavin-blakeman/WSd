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
//                      Copyright 2015, 2018 Gavin Blakeman.
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

#include "../Include/statemachine.h"

#include "include/database.h"
#include "include/settings.h"

#include <GCL>

#include <chrono>
#include <ctime>
#include <thread>

namespace WSd
{
  /// @brief Constructor for the state machine class.
  /// @param[in] np: The parent object of this instance.
  /// @param[in] sid: The <std::uint32_t> site ID.
  /// @param[in] iid: The <std::uint32_t> instrument ID.
  // 2015-05-17/GGB - Function created.

  CStateMachine::CStateMachine(QObject *np, std::uint32_t sid, std::uint32_t iid)
    : siteID(sid), instrumentID(iid), parent(np), pollTimer(nullptr)
  {
    tcpSocket = new tcp::CTCPSocket(parent, siteID, instrumentID);

    std::this_thread::sleep_for(std::chrono::seconds(60));

    pollTimer = new QTimer();
    connect(pollTimer, SIGNAL(timeout()), this, SLOT(pollModeTimer()));
    pollTimer->setInterval(WCL::settings::settings.value(WCL::settings::WS_POLLINTERVAL, 5).toInt() * 60000);

    WCL::database.connectToDatabase();
  }

  /// Destructor - Frees dynamically allocated objects
  //
  // 2015-05-17/GGB - Function created.

  CStateMachine::~CStateMachine()
  {
    if (pollTimer)
    {
      delete pollTimer;
      pollTimer = nullptr;
    };

    if (tcpSocket)
    {
      delete tcpSocket;
      tcpSocket = nullptr;
    }
  }

  /// Slot for the poll mode timer.
  //
  // 2015-05-17/GGB - Function created.

  void CStateMachine::pollModeTimer()
  {
    static bool timeUpdated = false;
    uint16_t dateValue, timeValue;
    double t1, t2;

    TRACEENTER;
    try
    {
      DEBUGMESSAGE("Connecting to database");
      WCL::database.openDatabase();
      std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    catch(...)
    {
      GCL::logger::defaultLogger().logMessage(GCL::logger::error, "Unable to connect to database.");
    };


    GCL::logger::defaultLogger().logMessage(GCL::logger::debug, "Polling Weather System Device.");

    tcpSocket->readArchive();

    std::time_t time = std::time(&time);
    struct tm *currentTime = std::localtime(&time);

    if ( (currentTime->tm_hour == 0) && (currentTime->tm_min < 10) && (!timeUpdated))
    {
      timeUpdated = tcpSocket->setTime();
    }
    else if ( (currentTime->tm_hour == 23) && (timeUpdated) )
    {
      timeUpdated = false;
    }
    else
    {
      WCL::database.lastWeatherRecord(siteID, instrumentID, dateValue, timeValue);
      dateValue = currentTime->tm_hour * 60 + currentTime->tm_min;                 // Time in minutes after start of day
      timeValue = (timeValue / 100) * 60 + (timeValue % 100);                                  // Convert time to minutes.

      t1 = std::abs(720 - dateValue);
      t2 = std::abs(720 - timeValue);

      if (std::abs(t1 - t2) > 5)        // If time difference greater than 5 minutes.
      {
        timeUpdated = tcpSocket->setTime();
      }
    }

    WCL::database.closeDatabase();

    TRACEEXIT;
  }

  /// Function to start the poll mode.
  //
  // 2015-04-11/GGB - Function created.

  void CStateMachine::start()
  {
    pollTimer->start();
  }

  void CStateMachine::stop()
  {
    pollTimer->stop();
  }

} // namespace OCWS
