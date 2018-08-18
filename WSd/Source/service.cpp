//*********************************************************************************************************************************
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

#include "../Include/service.h"

#include <Qt>
#include <QCoreApplication>
#include <QtCore>
#include <QTimer>
#include <QtNetwork>

#include "database.h"
#include "settings.h"

#include <GCL>

namespace WSd
{
    // Software version information

  int const MAJORVERSION	= 2015;       // Major version (year)
  int const MINORVERSION	= 5;         // Minor version (month)
  int const BUILDNUMBER = 0x0056;
  std::string const BUILDDATE(__DATE__);

  /// @brief Returns the copyright string.
  /// @returns The copyright string
  /// @throws None.
  /// @version 2015-05-17/GGB - Function created.

  std::string getCopyrightString()
  {
    QFile file(":/text/copyright_licence.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
      return std::string("");
    else
    {
      QTextStream data(&file);
      QString copyrightString = data.readAll();
      return copyrightString.toStdString();
    };
  }

  /// Function to return the release string.
  //
  // 2015-05-17/GGB - Function created.

  std::string getReleaseString()
  {
    char szString[50];

    sprintf(szString, "%04i.%02i.%04X", MAJORVERSION, MINORVERSION, BUILDNUMBER);

    return std::string(szString);
  }

  /// Function to return the build date.
  //
  // 2015-05-17/GGB - Function created.

  std::string getReleaseDate()
  {
    return BUILDDATE;
  }

  namespace service
  {

    /// Constructor for the service.
    ///
    // 2015-05-17/GGB - Function created.

    CWSService::CWSService(int argc, char **argv, unsigned long sid, unsigned long iid)
      : QtService<QCoreApplication>(argc, argv, "WSd"), stateMachine(nullptr), siteID(sid), instrumentID(iid)
    {
      TRACEENTER;
      application()->setOrganizationName(VWL::settings::ORG_NAME);
      application()->setApplicationName(VWL::settings::APPL_NAME);

      setServiceDescription("Weather Station - Daemon");

      TRACEEXIT;
    }

    /// Destructor for the class.
    //
    // 2015-05-17/GGB - Function created.

    CWSService::~CWSService()
    {
      if (stateMachine)
      {
        delete stateMachine;
        stateMachine = nullptr;
      }
    }

    /// Function to resume the daemon
    //
    // 2015-05-28/GGB - Function created.

    void CWSService::resume()
    {
      GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Daemon restarted.");
      stateMachine->start();
    }

    /// This is the main part of the service. All the code for the service creation needs to go in here.
    //
    // 2014-07-24/GGB - Function created.

    void CWSService::start()
    {
      TRACEENTER;
      std::ostringstream os;

        /* Write some messages for the user. */

      GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Application: WSd.");
      GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Copyright: Gavin Blakeman 2015.");
      GCL::logger::defaultLogger().logMessage(GCL::logger::info, "License: GPLv2.");
      os << "Release Number: " << getReleaseString() << ". Release Date: " << getReleaseDate();
      GCL::logger::defaultLogger().logMessage(GCL::logger::info, os.str());

        // Create the state machine

      DEBUGMESSAGE("Creating state machine...");
      stateMachine = new CStateMachine(this, siteID, instrumentID);
      DEBUGMESSAGE("State machine created.");

        /* Indicate that the service is starting. */

      GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Daemon Started.");
      stateMachine->start();

      TRACEEXIT;
    }

    /// Function to stop the daemon
    //
    // 2015-05-28/GGB - Function created.

    void CWSService::stop()
    {
      std::cout << "Stop Daemon" << std::endl;

      stateMachine->stop();
    }
  }   // namespace service
}   // namespace WSd
