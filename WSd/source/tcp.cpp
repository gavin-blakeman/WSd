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

#include "include/tcp.h"

  // Standard C++ library header files

#include <chrono>
#include <cstdint>
#include <ctime>
#include <thread>

  // Miscellaneous library header files

#include <ACL>
#include <WCL>

namespace WSd
{

  CTCPSocket::CTCPSocket(QObject *parent, std::uint32_t sid, std::uint32_t iid)
    : siteID(sid), instrumentID(iid), QTcpSocket(parent)
  {
    ipAddress = WCL::settings::settings.value(WCL::settings::WS_IPADDRESS, "192.168.8.129").toString();
    port = WCL::settings::settings.value(WCL::settings::WS_PORT, 22222).toInt();
  }

  /// @brief Command to request the start and end archive pointers from the WeatherLinkIP module.
  /// @throws
  /// @version 2015-05-17/GGB - Function created.

  bool CTCPSocket::readArchive()
  {
    char command[WCL::WL_MTU];
    size_t index;
    std::uint16_t date;
    std::uint16_t time;
    struct std::tm timeDate;
    std::uint16_t CRC;
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
          readData(static_cast<char *>(command), WCL::WL_MTU);
          exitWhile = true;
        }
        else
        {
          loopCount++;
        };
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
          dataLength = readData(static_cast<char *>(command), WCL::WL_MTU);
          index = 0;
          command[index++] = reinterpret_cast<char *>(&date)[0];
          command[index++] = reinterpret_cast<char *>(&date)[1];
          command[index++] = reinterpret_cast<char *>(&time)[0];
          command[index++] = reinterpret_cast<char *>(&time)[1];

          CRC = WCL::calculateCRC(reinterpret_cast<uint8_t *>(&command), 0, 4);
          command[index++] = reinterpret_cast<char *>(&CRC)[1];
          command[index++] = reinterpret_cast<char *>(&CRC)[0];
          writeData(command, index);

          if (waitForReadyRead(5000))
          {
            dataLength = readData(static_cast<char *>(command), WCL::WL_MTU);

            if (dataLength == 7 && command[0] == WCL::wlACK)
            {
              WCL::SDMPAFTResponse *response = reinterpret_cast<WCL::SDMPAFTResponse *>(command + 1);

              std::uint16_t pageCount = response->pages;
              std::uint8_t firstRecord = response->firstRecord;

              DEBUGMESSAGE("Reading: " + std::to_string(pageCount) + " Pages from WeatherView.");

              while (pageCount > 0)
              {
                index = 0;
                command[index++] = WCL::wlACK;
                writeData(command, index);

                if (waitForReadyRead(5000))
                {
                  dataLength = readData(static_cast<char *>(command), WCL::WL_MTU);
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
              };

              DEBUGMESSAGE("Completed Reading Pages from WeatherView.");

              INFOMESSAGE(std::to_string(recordCount) + " records written to database.");
            };
          }
          else
          {
            ERRORMESSAGE("No response from WeatherLinkIP module.");
          };
        }
        else
        {
          ERRORMESSAGE("No response from WeatherLinkIP module.");
        };
      }
      else
      {
        ERRORMESSAGE("No response from WeatherLinkIP module.");
      };

      disconnectFromHost();
    }
    else
    {
      ERRORMESSAGE("Unable to connect to WeatherLinkIP module.");
    };

    return returnValue;
  }

  /// @brief    Sets the time on the weather station.
  /// @throws
  /// @version  2015-06-04/GGB - Function created.

  bool CTCPSocket::setTime()
  {
    std::time_t time = std::time(&time);
    struct tm *currentTime = std::localtime(&time);
    char command[WCL::WL_MTU];
    size_t index;
    bool exitWhile = false;
    int loopCount = 0;
    std::uint16_t CRC;
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
          auto bytesRead = readData(static_cast<char *>(command), WCL::WL_MTU);
          if (bytesRead != 0)
          {
            exitWhile = true;
          }
          else
          {
            loopCount++;
          }
        }
        else
        {
          loopCount++;
        };
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
          dataLength = readData(static_cast<char *>(command), WCL::WL_MTU);
          index = 0;
          command[index++] = static_cast<uint8_t>(currentTime->tm_sec);
          command[index++] = static_cast<uint8_t>(currentTime->tm_min);
          command[index++] = static_cast<uint8_t>(currentTime->tm_hour);
          command[index++] = static_cast<uint8_t>(currentTime->tm_mday);
          command[index++] = static_cast<uint8_t>(currentTime->tm_mon + 1);
          command[index++] = static_cast<uint8_t>(currentTime->tm_year);

          CRC = WCL::calculateCRC(reinterpret_cast<uint8_t *>(&command), 0, 6);
          command[index++] = reinterpret_cast<char *>(&CRC)[1];
          command[index++] = reinterpret_cast<char *>(&CRC)[0];
          writeData(command, index);
          if (waitForReadyRead(5000))
          {
            dataLength = readData(static_cast<char *>(command), WCL::WL_MTU);
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
      INFOMESSAGE("Console time updated.");
    }
    else
    {
      if (loopCount < 3)
      {
        INFOMESSAGE("Console time not updated.");
      }
      else
      {
        ERRORMESSAGE("Unable to connect to console.");
      }

    }

    return returnValue;
  }

  /// @brief      Set the logging interval of the logger.
  /// @param[in]  period: The logging period to set.
  /// @returns    true if succesfull.
  /// @version    2020-10-25/GGB - Function created.

  bool CTCPSocket::setInterval(std::uint8_t period)
  {
    char command[WCL::WL_MTU];
    size_t index;
    bool exitWhile = false;
    int loopCount = 0;
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

        // Try to wake the console up.

      while ((loopCount < 3) && !exitWhile)
      {
        writeData(command, index);
        if (waitForReadyRead(1000))
        {
          auto bytesRead = readData(static_cast<char *>(command), WCL::WL_MTU);
          if (bytesRead != 0)
          {
            exitWhile = true;
          }
          else
          {
            loopCount++;
          }
        }
        else
        {
          loopCount++;
        };
      };

      if (loopCount < 3)
      {
        for (index = 0; index < sizeof(WCL::commandSETTIME); index++)
        {
          command[index] = WCL::commandSETPER[index];
        };

        switch (period)
        {
          case 1:
          {
            command[index++] = '1';
            break;
          };
          case 5:
          {
            command[index++] = '5';
            break;
          };
          case 10:
          {
            command[index++] = '1';
            command[index++] = '0';
            break;
          };
          case 15:
          {
            command[index++] = '1';
            command[index++] = '5';
            break;
          };
          case 30:
          {
            command[index++] = '3';
            command[index++] = '0';
            break;
          };
          case 60:
          {
            command[index++] = '6';
            command[index++] = '0';
            break;
          };
          case 120:
          default:
          {
            command[index++] = '1';
            command[index++] = '2';
            command[index++] = '0';
            break;
          };
        };

        command[index++] = WCL::wlLF;
        writeData(command, index);

        if (waitForReadyRead(5000))
        {
          dataLength = readData(static_cast<char *>(command), WCL::WL_MTU);
          if (dataLength == 1 && command[0] == WCL::wlACK)
          {
            returnValue = true;
          };
        };
      };
    };

    if (returnValue)
    {
      INFOMESSAGE("Console archive interval updated.");
    }
    else
    {
      if (loopCount < 3)
      {
        INFOMESSAGE("Console archive interval not updated.");
      }
      else
      {
        ERRORMESSAGE("Unable to connect to console.");
      }

    }

    return returnValue;
  }

}   // namespace WSd
