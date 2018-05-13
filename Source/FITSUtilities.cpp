//*********************************************************************************************************************************
//
// PROJECT:							Astronomy Class Library
// FILE:								FITSUtilities
// SUBSYSTEM:						FITS helper classes
// LANGUAGE:						C++
// TARGET OS:						WINDOWS/UNIX/LINUX/MAC
// LIBRARY DEPENDANCE:	boost
// NAMESPACE:						ACL
// AUTHOR:							Gavin Blakeman.
// LICENSE:             GPLv2
//
//                      Copyright 2011-2018 Gavin Blakeman.
//                      This file is part of the Astronomy Class Library (ACL)
//
//                      ACL is free software: you can redistribute it and/or modify it under the terms of the GNU General
//                      Public License as published by the Free Software Foundation, either version 2 of the License, or
//                      (at your option) any later version.
//
//                      ACL is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the
//                      implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//                      for more details.
//
//                      You should have received a copy of the GNU General Public License along with ACL.  If not,
//                      see <http://www.gnu.org/licenses/>.
//
// OVERVIEW:						Implements classes for interfacing to FITS files. This file does not overlap the FITSIO library, but
//                      provides additional support for these library.
//
// CLASSES INCLUDED:    None.
//
// CLASS HIERARCHY:     None.
//
// HISTORY:             2018-05-12 GGB - Bug fixes
//                      2015-09-22 GGB - AIRDAS 2015.09 release
//                      2013-09-30 GGB - AIRDAS 2013.09 release.
//                      2013-03-22 GGB - AIRDAS 2013.03 release.
//                      2013-01-20 GGB - AIRDAS 0000.00 release.
//                      2011-07-14 GGB - Development of classes for AIRDAS
//
//*********************************************************************************************************************************

#include "../Include/FITSUtilities.h"

  // ACL Libraries

#include "../Include/constants.h"

  // Standard library

#include <exception>
#include <iostream>
#include <iomanip>
#include <list>
#include <sstream>

  // Boost library

#include <boost/algorithm/string.hpp>
#include "boost/format.hpp"
#include "boost/tokenizer.hpp"

namespace ACL
{

  /// @brief Parses a time give as per the FITS standard for DATE and DATE-OBS
  /// @param[in] dateTime - The string containing the date time.
  /// @param[out] time - The converted tome.
  /// @details The following formats are specified in the FITS standard.
  ///           @li '14/10/96' - Original format, means 14 October 1996. (length = 8) (Only for century 1900)
  ///           @li '1996-10-14' (length = 10)
  ///           @li '1996-10-14T10:14:36.123' (length >= 19)
  ///          The function converts all the obove types of string into valid data.
  /// @returns true - The values contained in *time is valid.
  /// @returns false - The values in *time is not valid.
  /// @throws
  /// @version 2017-07-29/GGB - Function created.

  bool parseDATE_OBS(std::string const &dateTime, struct std::tm *time)
  {
    bool returnValue = false;
    bool error = false;
    std::istringstream inputStream(dateTime);

    try
    {
      if (dateTime.length() <= 8)
      {
        inputStream >> std::get_time(time, "%d//%m//%y");
        time->tm_year += 1900;
      }
      else if (dateTime.length() <= 10)
      {
        inputStream >> std::get_time(time, "%Y-%m-%d");
      }
      else if (dateTime.length() <= 19)
      {
        inputStream >> std::get_time(time, "%Y-%m-%dT%H:%M:%S");
      }
      else
      {
        /// @todo Code for ss.sss values.
      }
      returnValue = true;
    }
    catch(...)
    {
      returnValue = false;
    };

      // Checks that the values are reasonable.

    if (returnValue)
    {
        // All values are year are probably correct.

      switch (time->tm_mon)
      {
        case 0:   // January
        case 2:   // March
        case 4:   // May
        case 6:   // July
        case 7:   // August
        case 9:  // October
        case 11:  // December
        {
          if ( (time->tm_mday < 1) || (time->tm_mday > 31))
          {
            error = true;
          };
          break;
        }
        case 3:   // April
        case 5:   // June
        case 8:   // September
        case 10:  // November
        {
          if ( (time->tm_mday < 1) || (time->tm_mday > 30))
          {
            error = true;
          };
          break;
        }
        case 1:
        {
          if ( (time->tm_mday < 1) || (time->tm_mday > 29))
          {
            error = true;
          };
          break;
        }
        default:
        {
          break;
        }
      }

      if (!error)
      {
        if ( (time->tm_hour < 0) || (time->tm_hour > 23) )
        {
          error = true;
        }
        else if ((time->tm_min < 0) || (time->tm_min > 59))
        {
          error = true;
        }
        else if ((time->tm_sec < 0) || (time->tm_sec > 60))     // Remember when extra seconds are added this is possible.
        {
          error = true;
        }
      }

      if (error)
      {
        returnValue = false;
      }
    };

    return returnValue;
  }

  /// @brief Parses an RA value represented as a string to a floating point value. (HH MM SS.sss)
  /// @param[in] value - The string to parse
  /// @returns The value retrieved from the operation in radians
  /// @throws std::invalid_argument
  /// @version 2018-05-12/GGB - Updated to return radians.
  /// @version 2017-08-02/GGB - Function created.

  FP_t parseRA(std::string value)
  {
    FP_t returnValue;
    std::uint_fast8_t index = 0;

    boost::char_separator<char> sep(" ");
    boost::erase_all(value, "'");
    boost::tokenizer<boost::char_separator<char>> tokens(value, sep);

    for (auto element : tokens)
    {
      switch (index)
      {
        case 0:
        {
          returnValue = std::stod(element);
          if ( (returnValue < 0) || (returnValue > 23))
          {
            throw(std::invalid_argument("Hours value not correctly delineated."));
          }
          else
          {
            returnValue *= DEGREE_PER_HOUR;   // Convert to degrees.
          };
          break;
        }
        case 1:
        {
          double temp = std::stod(element);
          if ( (temp < 0) || (temp > 59))
          {
            throw(std::invalid_argument("Minutes value not correctly delineated."));
          };
          returnValue += temp / 60;
          break;
        }
        case 2:
        {
          double temp = std::stod(element);
          if ( (temp < 0) || (temp > 59))
          {
            throw(std::invalid_argument("Seconds value not correctly delineated."));
          };
          returnValue += temp / 3600;
          break;
        }
        case 3:
        default:
        {
          throw(std::invalid_argument("Too many number groups when converting RA."));
          break;
        };
      };
      ++index;
    };

      // Return value now in form hh.ddddd. Need to change this to radians.

    return (returnValue * D_D2R);
  }

  /// @brief Parses a Declination value represented as a string to a floating point value. (SDD MM SS.sss)
  /// @param[in] value - The string to parse
  /// @returns The value retrieved from the argument in radians.
  /// @throws std::invalid_argument
  /// @version 2018-05-12/GGB - Updated to return radians.
  /// @version 2017-08-02/GGB - Function created.

  FP_t parseDEC(std::string value)
  {
    FP_t returnValue;
    std::uint_fast8_t index = 0;

    boost::char_separator<char> sep(" ");
    boost::erase_all(value, "'");
    boost::tokenizer<boost::char_separator<char>> tokens(value, sep);

    for (auto element : tokens)
    {
      switch (index)
      {
        case 0:
        {
          returnValue = std::stod(element);
          if ( (returnValue < -90) || (returnValue > +90))
          {
            throw(std::invalid_argument("Hours value not correctly delineated."));
          }
          break;
        }
        case 1:
        {
          double temp = std::stod(element);
          if ( (temp < 0) || (temp > 59))
          {
            throw(std::invalid_argument("Minutes value not correctly delineated."));
          };
          returnValue += temp / 60;
          break;
        }
        case 2:
        {
          double temp = std::stod(element);
          if ( (temp < 0) || (temp > 59))
          {
            throw(std::invalid_argument("Seconds value not correctly delineated."));
          };
          returnValue += temp / 3600;
          break;
        }
        case 3:
        default:
        {
          throw(std::invalid_argument("Too many number groups when converting RA."));
          break;
        };
      };
      ++index;
    };

    return (returnValue * D_D2R);
  }

  /// @brief Parses a Latitude value represented as a string to a floating point value. (sDD MM SS.sss)
  /// @param[in] value - The string to parse
  /// @param[out] ra - The value retrieved from the operation.
  /// @returns true = success
  /// @returns false = failure
  /// @note 1. Calls parseDEC().
  /// @throws std::invalid_argument
  /// @version 2017-08-02/GGB - Function created.

  FP_t parseLatitude(std::string const &value)
  {
    return parseDEC(value);
  }

  /// @brief Parses a Longitudevalue represented as a string to a floating point value. (sDDD MM SS.sss)
  /// @param[in] value - The string to parse
  /// @param[out] ra - The value retrieved from the operation.
  /// @returns true = success
  /// @returns false = failure
  /// @note 1. Calls parseDEC().
  /// @throws std::invalid_argument
  /// @version 2017-08-02/GGB - Function created.

  FP_t parseLongitude(std::string value)
  {
    FP_t returnValue;
    std::uint_fast8_t index = 0;

    boost::char_separator<char> sep(" ");
    boost::erase_all(value, "'");
    boost::tokenizer<boost::char_separator<char>> tokens(value, sep);

    for (auto element : tokens)
    {
      switch (index)
      {
        case 0:
        {
          returnValue = std::stod(element);
          if ( (returnValue < -180) || (returnValue > +180))
          {
            throw(std::invalid_argument("Hours value not correctly delineated."));
          }
          break;
        }
        case 1:
        {
          double temp = std::stod(element);
          if ( (temp < 0) || (temp > 59))
          {
            throw(std::invalid_argument("Minutes value not correctly delineated."));
          };
          returnValue += temp / 60;
          break;
        }
        case 2:
        {
          double temp = std::stod(element);
          if ( (temp < 0) || (temp > 59))
          {
            throw(std::invalid_argument("Seconds value not correctly delineated."));
          };
          returnValue += temp / 3600;
          break;
        }
        case 3:
        default:
        {
          throw(std::invalid_argument("Too many number groups when converting RA."));
          break;
        };
      };
      ++index;
    };

    return returnValue;
  }

  /// @brief Outputs the passed date time as a string value.
  /// @param[in] yy - year
  /// @param[in] mm - Month
  /// @param[in] dd - Day
  /// @param[in] hh - Hours
  /// @param[in] MM - minutes
  /// @param[in] ss - Seconds
  /// @returns A string containing the date/time (yyyy-mm-ddThh:mm:ss)
  /// @throws None.
  /// @version 2016-04-16/GGB - Updated to use boost::format to do the string formatting. (Bug 1571151)
  /// @version 2011-12-28/GGB - Function created.

  std::string sprintFITSDateTime(int const &yy, int const &mm, int const &dd, int const &hh, int const &MM, FP_t const &ss)
  {
    std::ostringstream o;

    o << boost::format("%1$04d-%2$02d-%3$02dT%4$02d:%5$02d:%6$02d") % yy % mm % dd % hh % MM % ss;

    return o.str();
  }

  /// @brief Outputs the passed date time as a string value.
  /// @param[in] at - The time to convert to a string.
  /// @returns A string containing the date/time (yyyy-mm-ddThh:mm:ss)
  /// @throws None.
  /// @version 2011-12-29/GGB - Function created.

  std::string sprintFITSDateTime(CAstroTime const &at)
  {
    struct tm cal;
    int year, month;

    at.UTC().gregorianDate(&cal);
    year = cal.tm_year + 1900;
    month = cal.tm_mon + 1;

    return sprintFITSDateTime(year, month, cal.tm_mday, cal.tm_hour, cal.tm_min, cal.tm_sec);
  }

}    // namespace ACL
