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
// OVERVIEW:            Implements the main(...) function
//
// HISTORY:             2015-05-17/GGB - Development of classes for WSd
//
//*********************************************************************************************************************************

#include "../Include/tcp.h"

#include "include/database.h"
#include "include/settings.h"

#include "include/GeneralFunctions.h"
#include "include/WeatherLinkIP.h"

#include <chrono>
#include <ctime>
#include <thread>

#include <ACL>

namespace WSd
{
  namespace tcp
  {

    CTCPSocket::CTCPSocket(QObject *parent, unsigned long sid, unsigned long iid)
      : siteID(sid), instrumentID(iid), QTcpSocket(parent)
    {
      ipAddress = WCL::settings::settings.value(WCL::settings::WS_IPADDRESS, "192.168.8.129").toString();
      port = WCL::settings::settings.value(WCL::settings::WS_PORT, 22222).toInt();
    }

    /// @brief Command to request the start and end archive pointers from the WeatherLinkIP module.
    //
    /// @version 2015-05-17/GGB - Function created.

    bool CTCPSocket::readArchive()
    {
      char command[WL_MTU];
      size_t index;
      uint16_t date;
      uint16_t time;
      struct std::tm timeDate;
      uint16_t CRC;
      bool returnValue = false;
      bool exitWhile;
      int loopCount;
      qint64 dataLength;
      int recordCount = 0;

      DEBUGMESSAGE("Reading last weather record.");
      WCL::database.lastWeatherRecord(siteID, instrumentID, date, time);

      ACL::TJD JD = ACL::TJD(static_cast<ACL::FP_t>(date) + ACL::MJD0);

      if (JD.gregorianDate(&timeDate))
      {
        date = timeDate.tm_mday + (timeDate.tm_mon + 1) * 32 + (timeDate.tm_year - 100) * 512;
      }
      else
      {
        date = 0;
        time = 0;
      };

      abort();
      connectToHost(ipAddress, port);

      if (waitForConnected(1000))
      {
        index = 0;
        command[index++] = WCL::wlLF;

        loopCount = 0;
        exitWhile = false;

        while ((loopCount < 3) && !exitWhile)
        {
          writeData(command, index);
          if (waitForReadyRead(1000))
          {
            readData(static_cast<char *>(command), WL_MTU);
            exitWhile = true;
          }
          else
          {
            loopCount++;
          }
        };

        if (loopCount < 3)
        {
          for (index = 0; index < sizeof(WCL::commandDMPAFT); index++)
          {
            command[index] = WCL::commandDMPAFT[index];
          };
          command[index++] = WCL::wlLF;

          writeData(command, index);

          if (waitForReadyRead(5000))
          {
            dataLength = readData(static_cast<char *>(command), WL_MTU);
            index = 0;
            command[index++] = reinterpret_cast<char *>(&date)[0];
            command[index++] = reinterpret_cast<char *>(&date)[1];
            command[index++] = reinterpret_cast<char *>(&time)[0];
            command[index++] = reinterpret_cast<char *>(&time)[1];

            CRC = VWL::calculateCRC(reinterpret_cast<uint8_t *>(&command), 0, 4);
            command[index++] = reinterpret_cast<char *>(&CRC)[1];
            command[index++] = reinterpret_cast<char *>(&CRC)[0];
            writeData(command, index);

            if (waitForReadyRead(5000))
            {
              dataLength = readData(static_cast<char *>(command), WL_MTU);

              if (dataLength == 7 && command[0] == WCL::wlACK)
              {
                WCL::SDMPAFTResponse *response = reinterpret_cast<WCL::SDMPAFTResponse *>(command + 1);

                uint16_t pageCount = response->pages;
                uint8_t firstRecord = response->firstRecord;

                GCL::logger::defaultLogger().logMessage(GCL::logger::debug, "Reading: " + std::to_string(pageCount) + " Pages from WeatherView.");

                while (pageCount > 0)
                {
                  index = 0;
                  command[index++] = WCL::wlACK;
                  writeData(command, index);

                  if (waitForReadyRead(5000))
                  {
                    dataLength = readData(static_cast<char *>(command), WL_MTU);
                    WCL::SDumpPage *dataResponse = reinterpret_cast<WCL::SDumpPage *>(command);

                    for (index = firstRecord; index < 5; index++)
                    {
                      if (WCL::database.insertRecord(siteID, instrumentID, dataResponse->record[index]))
                      {
                        recordCount++;
                      };
                    };
                    firstRecord = 0;
                  }
                  else
                  {
                    break;
                  };
                };

                  // All records processed.

                if (pageCount == 0)
                {
                  returnValue = true;
                }

                GCL::logger::defaultLogger().logMessage(GCL::logger::debug, "Completed Reading Pages from WeatherView.");

                GCL::logger::defaultLogger().logMessage(GCL::logger::info, std::to_string(recordCount) + " records written to database.");
              };
            }
            else
            {
              GCL::logger::defaultLogger().logMessage(GCL::logger::error, "No response from WeatherLinkIP module.");
            }
          }
          else
          {
            GCL::logger::defaultLogger().logMessage(GCL::logger::error, "No response from WeatherLinkIP module.");
          }
        }
        else
        {
          GCL::logger::defaultLogger().logMessage(GCL::logger::error, "No response from WeatherLinkIP module.");
        }

        disconnectFromHost();
      }
      else
      {
        GCL::logger::defaultLogger().logMessage(GCL::logger::error, "Unable to connect to WeatherLinkIP module.");
      }

      return returnValue;
    }

    /// Sets the time on the weather station.
    //
    // 2015-06-04/GGB - Function created.

    bool CTCPSocket::setTime()
    {
      std::time_t time = std::time(&time);
      struct tm *currentTime = std::localtime(&time);
      char command[WL_MTU];
      size_t index;
      bool exitWhile = false;
      int loopCount = 0;
      uint16_t CRC;
      qint64 dataLength;
      bool returnValue = false;

      abort();
      connectToHost(ipAddress, port);
      if (waitForConnected(1000))
      {
        index = 0;
        command[index++] = WCL::wlLF;

        loopCount = 0;
        exitWhile = false;

        while ((loopCount < 3) && !exitWhile)
        {
          writeData(command, index);
          if (waitForReadyRead(1000))
          {
            readData(static_cast<char *>(command), WL_MTU);
            exitWhile = true;
          }
          else
          {
            loopCount++;
          }
        };

        if (loopCount < 3)
        {
          for (index = 0; index < sizeof(WCL::commandSETTIME); index++)
          {
            command[index] = WCL::commandSETTIME[index];
          };
          command[index++] = WCL::wlLF;
          writeData(command, index);

          if (waitForReadyRead(5000))
          {
            dataLength = readData(static_cast<char *>(command), WL_MTU);
            index = 0;
            command[index++] = static_cast<uint8_t>(currentTime->tm_sec);
            command[index++] = static_cast<uint8_t>(currentTime->tm_min);
            command[index++] = static_cast<uint8_t>(currentTime->tm_hour);
            command[index++] = static_cast<uint8_t>(currentTime->tm_mday);
            command[index++] = static_cast<uint8_t>(currentTime->tm_mon + 1);
            command[index++] = static_cast<uint8_t>(currentTime->tm_year);

            CRC = VWL::calculateCRC(reinterpret_cast<uint8_t *>(&command), 0, 6);
            command[index++] = reinterpret_cast<char *>(&CRC)[1];
            command[index++] = reinterpret_cast<char *>(&CRC)[0];
            writeData(command, index);
            if (waitForReadyRead(5000))
            {
              dataLength = readData(static_cast<char *>(command), WL_MTU);
              if (dataLength == 1 && command[0] == WCL::wlACK)
              {
                returnValue = true;
              };
            };

          };
        };
      };

      if (returnValue)
      {
        GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Console time updated.");
      }
      else
      {
        GCL::logger::defaultLogger().logMessage(GCL::logger::info, "Console time not updated.");
      }

      return returnValue;
    }


  }   // namespace tcp
}   // namespace WSd
